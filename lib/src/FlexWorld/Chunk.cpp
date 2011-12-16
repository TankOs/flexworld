#include <FlexWorld/Chunk.hpp>

#include <cstring>

namespace flex {

Chunk::Chunk( const Vector& size ) :
	m_size( size ),
	m_blocks( new ClassCache::IdType[size.x * size.y * size.z] )
{
	clear();
}

Chunk::~Chunk() {
	delete[] m_blocks;
}

void Chunk::clear() {
	memset( m_blocks, 0, sizeof( ClassCache::IdType ) * m_size.x * m_size.y * m_size.z );
}

const Chunk::Vector& Chunk::get_size() const {
	return m_size;
}

ClassCache::IdType Chunk::get_block( const Vector& pos ) const {
	if( pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z ) {
		return 0;
	}

	return m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x];
;
}

void Chunk::set_block( const Vector& pos, ClassCache::IdType class_id ) {
	if( pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z ) {
		return;
	}

	m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] = class_id;
}

void Chunk::f() {
}

}
