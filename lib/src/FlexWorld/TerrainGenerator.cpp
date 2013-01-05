#ifndef NDEBUG
#define OUTPUT_DEBUG_IMAGE
#endif

#include <FlexWorld/TerrainGenerator.hpp>
#include <FlexWorld/Planet.hpp>

#ifdef OUTPUT_DEBUG_IMAGE
	#include <SFML/Graphics/Image.hpp> // XXX 
#endif

#include <libnoise/noise.h>
#include <stdexcept>
#include <cassert>
#include <iostream> // XXX 

namespace fw {

TerrainGenerator::TerrainGenerator( const Class& default_cls ) :
	m_default_cls( &default_cls ),
	m_seed( 0 ),
	m_base_height( 0 ),
	m_max_height( 0 )
{
}

const Class& TerrainGenerator::get_default_class() const {
	return *m_default_cls;
}

std::size_t TerrainGenerator::get_num_layers() const {
	return m_layers.size();
}

void TerrainGenerator::add_layer( const Layer& layer ) {
	m_layers.push_back( layer );
}

const TerrainGenerator::Layer& TerrainGenerator::get_layer( std::size_t index ) const {
	assert( index < m_layers.size() );

	return m_layers[index];
}

int TerrainGenerator::get_seed() const {
	return m_seed;
}

void TerrainGenerator::set_seed( int seed ) {
	m_seed = seed;
}

uint32_t TerrainGenerator::get_base_height() const {
	return m_base_height;
}

void TerrainGenerator::set_base_height( uint32_t height ) {
	m_base_height = height;
}

uint32_t TerrainGenerator::get_maximum_height() const {
	return m_max_height;
}

void TerrainGenerator::set_maximum_height( uint32_t height ) {
	m_max_height = height;
}

void TerrainGenerator::generate( Planet& planet, const util::Cuboid<uint32_t>& cuboid ) {
	assert( cuboid.x + cuboid.width <= planet.get_size().x * planet.get_chunk_size().x );
	assert( cuboid.y + cuboid.height <= planet.get_size().y * planet.get_chunk_size().y );
	assert( cuboid.z + cuboid.depth <= planet.get_size().z * planet.get_chunk_size().z );
	assert( cuboid.y + m_base_height + m_max_height <= planet.get_size().y * planet.get_chunk_size().y );

	// Prepare Perlin noise generator.
	using namespace noise::module;
	Perlin perlin;

	perlin.SetSeed( m_seed );
	perlin.SetPersistence( 1.1 );

	// Prepare loops.
	uint32_t max_x = cuboid.x + cuboid.width;
	uint32_t max_z = cuboid.z + cuboid.depth;
	Planet::Vector initial_chunk_pos( 0, 0, 0 );
	Chunk::Vector initial_block_pos( 0, 0, 0 );
	Chunk::Vector chunk_size = planet.get_chunk_size();
	double value = 0;
	uint32_t final_value = 0;
	uint32_t y = 0;

	// Get initial chunk and block positions.
	bool result = planet.transform(
		Planet::Coordinate(
			static_cast<float>( cuboid.x ),
			static_cast<float>( cuboid.y ),
			static_cast<float>( cuboid.z )
		),
		initial_chunk_pos,
		initial_block_pos
	);

	assert( result );
	if( !result ) {
		throw std::runtime_error( "Failed to transform planet coordinate." );
	}

	Planet::Vector chunk_pos = initial_chunk_pos;
	Chunk::Vector block_pos = initial_block_pos;
	double max_height = static_cast<double>( m_max_height );
	double base = static_cast<double>( m_base_height );

#ifdef OUTPUT_DEBUG_IMAGE
	sf::Image debug_image;
	debug_image.create( cuboid.width, cuboid.depth );
#endif

	for( uint32_t z = cuboid.z; z < max_z; ++z ) {
		for( uint32_t x = cuboid.x; x < max_x; ++x ) {
			value = perlin.GetValue(
				static_cast<double>( x ) / 800.0,
				0.1,
				static_cast<double>( z ) / 800.0
			);

			value = std::max( -1.0, std::min( 1.0, value ) );
			value = (value + 1.0) / 2.0f;
			value *= max_height;

#ifdef OUTPUT_DEBUG_IMAGE
			debug_image.setPixel(
				x,
				z,
				sf::Color(
					static_cast<sf::Uint8>( value * 255.0 ),
					static_cast<sf::Uint8>( value * 255.0 ),
					static_cast<sf::Uint8>( value * 255.0 )
				)
			);
#endif

			final_value = m_base_height + static_cast<uint32_t>( value );

			// Save blocks at planet.
			block_pos.y = initial_block_pos.y;
			chunk_pos.y = initial_chunk_pos.y;

			if( !planet.has_chunk( chunk_pos ) ) {
				planet.create_chunk( chunk_pos );
			}

			for(
				y = cuboid.y;
				y < final_value && y < cuboid.y + cuboid.height;
				++y
			) {
				// Skip base blocks. TODO
				if( y >= m_base_height ) {
					// Set the block.
					planet.set_block( chunk_pos, block_pos, *m_default_cls );
				}

				// Advance block.
				++block_pos.y;

				if( block_pos.y >= chunk_size.y ) {
					block_pos.y = 0;
					++chunk_pos.y;

					// Create chunk.
					if( !planet.has_chunk( chunk_pos ) ) {
						planet.create_chunk( chunk_pos );
					}
				}
			}

			++block_pos.x;
			if( block_pos.x >= chunk_size.x ) {
				block_pos.x = 0;
				++chunk_pos.x;
			}
		}

		chunk_pos.x = initial_chunk_pos.x;
		block_pos.x = initial_block_pos.x;

		++block_pos.z;
		if( block_pos.z >= chunk_size.z ) {
			block_pos.z = 0;
			++chunk_pos.z;
		}
	}

#ifdef OUTPUT_DEBUG_IMAGE
	debug_image.saveToFile( "debug.png" );
#endif
}

// --------------------------------------------------

TerrainGenerator::Layer::Layer() :
	min_height( 0 ),
	max_height( 0 ),
	cls( nullptr )
{
}

bool operator==( const TerrainGenerator::Layer& first, const TerrainGenerator::Layer& second ) {
	return
		first.min_height == second.min_height &&
		first.max_height == second.max_height &&
		first.cls == second.cls
	;
}

}
