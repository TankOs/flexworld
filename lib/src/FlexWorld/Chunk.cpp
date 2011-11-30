#if 0
#include <FlexWorld/Chunk.hpp>

#include <cstring>

namespace flex {

Chunk::Chunk( const Vector& size ) :
	m_size( size ),
	m_blocks( new ClassIdType[size.x * size.y * size.z] )
{
	clear();
}

Chunk::~Chunk() {
	delete[] m_blocks;
}

void Chunk::clear() {
	memset( m_blocks, 0, sizeof( ClassIdType ) * m_size.x * m_size.y * m_size.z );

	m_classes.clear();
	m_ids.clear();
}

const Vector& Chunk::get_size() const {
	return m_size;
}

void Chunk::set_block( const Vector& pos, const Class& cls ) {
	if( pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z ) {
		return;
	}

	// Try to get class ID for specified class if cached yet, else cache it.
}

}

#endif
