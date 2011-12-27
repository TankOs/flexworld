#include <FlexWorld/Planet.hpp>

#include <cassert>
#include <iostream>

namespace std {
	bool operator<( const flex::Planet::Vector& first, const flex::Planet::Vector& second ) {
		if( first.z < second.z ) {
			return true;
		}
		else if( first.z > second.z ) {
			return false;
		}

		if( first.y < second.y ) {
			return true;
		}
		else if( first.y > second.y ) {
			return false;
		}

		if( first.x < second.x ) {
			return true;
		}

		return false;
	}
}

namespace flex {

Planet::Planet( const std::string& id, const Vector& size, const Chunk::Vector& chunk_size ) :
	m_size( size ),
	m_chunk_size( chunk_size ),
	m_id( id )
{
}

Planet::~Planet() {
	clear();
}

void Planet::clear() {
	ChunkMap::iterator chunk_iter( m_chunks.begin() );
	ChunkMap::iterator chunk_iter_end( m_chunks.end() );
	
	for( ; chunk_iter != chunk_iter_end; ++chunk_iter ) {
		delete chunk_iter->second;
	}

	m_chunks.clear();
}

const std::string& Planet::get_id() const {
	return m_id;
}

const Planet::Vector& Planet::get_size() const {
	return m_size;
}

const Chunk::Vector& Planet::get_chunk_size() const {
	return m_chunk_size;
}

bool Planet::transform( const sf::Vector3f& coord, Vector& chunk_pos, Chunk::Vector& block_pos ) const {
	sf::Vector3<uint32_t> i_coord(
		static_cast<uint32_t>( coord.x ),
		static_cast<uint32_t>( coord.y ),
		static_cast<uint32_t>( coord.z )
	);

	chunk_pos.x = static_cast<ScalarType>( i_coord.x / m_chunk_size.x );
	chunk_pos.y = static_cast<ScalarType>( i_coord.y / m_chunk_size.y );
	chunk_pos.z = static_cast<ScalarType>( i_coord.z / m_chunk_size.z );

	block_pos.x = static_cast<Chunk::ScalarType>( i_coord.x % m_chunk_size.x );
	block_pos.y = static_cast<Chunk::ScalarType>( i_coord.y % m_chunk_size.y );
	block_pos.z = static_cast<Chunk::ScalarType>( i_coord.z % m_chunk_size.z );

	if( chunk_pos.x >= m_size.x || chunk_pos.y >= m_size.y || chunk_pos.z >= m_size.z ) {
		return false;
	}

	return true;
}

bool Planet::has_chunk( const Vector& position ) const {
	assert( position.x < m_size.x );
	assert( position.y < m_size.y );
	assert( position.z < m_size.z );

	ChunkMap::const_iterator iter( m_chunks.find( position ) );
	return iter != m_chunks.end();
}

void Planet::create_chunk( const Vector& pos ) {
	assert( pos.x < m_size.x );
	assert( pos.y < m_size.y );
	assert( pos.z < m_size.z );
	assert( m_chunks.find( pos ) == m_chunks.end() ); // TODO: Turn this into an exception?

	m_chunks[pos] = new Chunk( m_chunk_size );
}

std::size_t Planet::get_num_chunks() const {
	return m_chunks.size();
}

void Planet::set_block( const Vector& chunk_pos, const Chunk::Vector& block_pos, const Class& cls ) {
	assert( chunk_pos.x < m_size.x && chunk_pos.y < m_size.y && chunk_pos.z < m_size.z );
	assert( block_pos.x < m_chunk_size.x && block_pos.y < m_chunk_size.y && block_pos.z < m_chunk_size.z );

	// Get chunk.
	assert( has_chunk( chunk_pos ) );
	Chunk* chunk( m_chunks[chunk_pos] );

	// Cache class.
	ClassCache::IdType internal_id( m_class_cache.cache( cls ) );

	// If block was set before, forget old class.
	if( chunk->is_block_set( block_pos ) ) {
		m_class_cache.forget( m_class_cache.get_class( chunk->get_block( block_pos ) ) );
	}

	// Set block.
	chunk->set_block( block_pos, internal_id );
}

const Class* Planet::find_block( const Vector& chunk_pos, const Chunk::Vector& block_pos ) const {
	assert( chunk_pos.x < m_size.x && chunk_pos.y < m_size.y && chunk_pos.z < m_size.z );
	assert( block_pos.x < m_chunk_size.x && block_pos.y < m_chunk_size.y && block_pos.z < m_chunk_size.z );

	// Get chunk.
	ChunkMap::const_iterator chunk_iter( m_chunks.find( chunk_pos ) );
	assert( chunk_iter != m_chunks.end() );

	if( !chunk_iter->second->is_block_set( block_pos ) ) {
		return nullptr;
	}

	// Get class.
	return &m_class_cache.get_class( chunk_iter->second->get_block( block_pos ) );
}

void Planet::reset_block( const Vector& chunk_pos, const Chunk::Vector& block_pos ) {
	assert( chunk_pos.x < m_size.x && chunk_pos.y < m_size.y && chunk_pos.z < m_size.z );
	assert( block_pos.x < m_chunk_size.x && block_pos.y < m_chunk_size.y && block_pos.z < m_chunk_size.z );

	// Get chunk.
	assert( has_chunk( chunk_pos ) );
	Chunk* chunk( m_chunks[chunk_pos] );

	if( !chunk->is_block_set( block_pos ) ) {
		return;
	}

	// Forget class.
	m_class_cache.forget( m_class_cache.get_class( chunk->get_block( block_pos ) ) );

	// Reset block.
	chunk->reset_block( block_pos );
}

}
