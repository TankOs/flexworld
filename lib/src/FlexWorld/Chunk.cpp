#include <FlexWorld/Chunk.hpp>

#include <cstring>
#include <limits>
#include <cassert>

namespace flex {

const ClassCache::IdType Chunk::INVALID_ID = std::numeric_limits<ClassCache::IdType>::max();

Chunk::Chunk( const Vector& size ) :
	m_size( size ),
	m_blocks( new ClassCache::IdType[size.x * size.y * size.z] )
{
	assert( size.x > 0 && size.y > 0 && size.z > 0 );
	clear();
}

Chunk::~Chunk() {
	delete[] m_blocks;
}

void Chunk::clear() {
	memset( m_blocks, INVALID_ID, sizeof( ClassCache::IdType ) * m_size.x * m_size.y * m_size.z );
}

const Chunk::Vector& Chunk::get_size() const {
	return m_size;
}

ClassCache::IdType Chunk::get_block( const Vector& pos ) const {
	assert( pos.x < m_size.x || pos.y < m_size.y || pos.z < m_size.z );
	assert( m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] != INVALID_ID );

	return m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x];
;
}

void Chunk::set_block( const Vector& pos, ClassCache::IdType class_id ) {
	assert( pos.x < m_size.x || pos.y < m_size.y || pos.z < m_size.z );
	assert( class_id != INVALID_ID );

	m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] = class_id;
}

bool Chunk::is_block_set( const Vector& pos ) const {
	assert( pos.x < m_size.x && pos.y < m_size.y && pos.z < m_size.z );
	return m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] != INVALID_ID;
}

void Chunk::reset_block( const Vector& pos ) {
	assert( pos.x < m_size.x || pos.y < m_size.y || pos.z < m_size.z );
	assert( m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] != INVALID_ID );

	m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] = INVALID_ID;
}

}
