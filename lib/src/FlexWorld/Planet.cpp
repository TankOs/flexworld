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

const Chunk* Planet::get_chunk( const Vector& pos ) const {
	// Check valid position.
	if( pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z ) {
		return nullptr;
	}

	// Find chunk.
	ChunkMap::const_iterator c_iter( m_chunks.find( pos ) );
	return c_iter != m_chunks.end() ? c_iter->second : nullptr;
}

std::size_t Planet::get_num_chunks() const {
	return m_chunks.size();
}

bool Planet::transform_coordinate( const sf::Vector3f& coord, Vector& chunk_pos, Chunk::Vector& block_pos ) const {
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

const Class* Planet::get_block( const Coordinate& coord ) const {
	Vector chunk_pos;
	Chunk::Vector block_pos;

	if( !transform_coordinate( coord, chunk_pos, block_pos ) ) {
		return nullptr;
	}

	const Chunk* chunk( get_chunk( chunk_pos ) );
	if( chunk == nullptr ) {
		return nullptr;
	}

	// Nothing set?
	ClassCache::IdType class_id( chunk->get_block( block_pos ) );
	if( !class_id ) {
		return nullptr;
	}

	// Get class.
	const Class* cls( m_class_cache.get_class( class_id ) );
	assert( cls );
	if( !cls ) {
		return nullptr;
	}

	return cls;
}

const ClassCache& Planet::get_class_cache() const {
	return m_class_cache;
}

bool Planet::set_block( const Coordinate& coord, const Class& cls ) {
	Vector chunk_pos;
	Chunk::Vector block_pos;

	if( !transform_coordinate( coord, chunk_pos, block_pos ) ) {
		return false;
	}

	// Chunk must exist!
	ChunkMap::iterator c_iter( m_chunks.find( chunk_pos ) );
	if( c_iter == m_chunks.end() ) {
		return false;
	}

	// Forget old block's class if any.
	ClassCache::IdType old_class_id( c_iter->second->get_block( block_pos ) );

	if( old_class_id ) {
		const Class* old_class( m_class_cache.get_class( old_class_id ) );
		assert( old_class != nullptr );

		m_class_cache.forget( *old_class );
	}

	// Cache class.
	ClassCache::IdType class_id( m_class_cache.cache( cls ) );

	c_iter->second->set_block( block_pos, class_id );
	return true;
}

bool Planet::create_chunk( const Vector& pos ) {
	if( pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z ) {
		return false;
	}

	if( get_chunk( pos ) != nullptr ) {
		return false;
	}

	m_chunks[pos] = new Chunk( m_chunk_size );
	return true;
}

bool Planet::reset_block( const Coordinate& coord ) {
	Vector chunk_pos;
	Chunk::Vector block_pos;

	if( !transform_coordinate( coord, chunk_pos, block_pos ) ) {
		return false;
	}

	// Chunk must exist!
	ChunkMap::iterator c_iter( m_chunks.find( chunk_pos ) );
	if( c_iter == m_chunks.end() ) {
		return false;
	}

	// Forget old block's class if any.
	ClassCache::IdType class_id( c_iter->second->get_block( block_pos ) );

	if( !class_id ) {
		return false;
	}

	// Get class and decrease use counter.
	const Class* cls( m_class_cache.get_class( class_id ) );
	assert( cls );
	if( !cls ) {
		return false;
	}

	c_iter->second->set_block( block_pos, 0 );
	m_class_cache.forget( *cls );

	return true;
}

}
