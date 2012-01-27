#include <FlexWorld/Chunk.hpp>

#include <cstring>
#include <limits>
#include <cassert>

namespace flex {

const Chunk::Block Chunk::MAX_BLOCK_ID = static_cast<Block>( std::numeric_limits<Block>::max() - 1 );
const Chunk::Block Chunk::INVALID_BLOCK = std::numeric_limits<Block>::max();

Chunk::Chunk( const Vector& size ) :
	m_size( size ),
	m_blocks( new Block[size.x * size.y * size.z] )
{
	assert( size.x > 0 && size.y > 0 && size.z > 0 );
	clear();
}

Chunk::~Chunk() {
	delete[] m_blocks;
}

void Chunk::clear() {
	memset( m_blocks, INVALID_BLOCK, sizeof( Block ) * m_size.x * m_size.y * m_size.z );
}

const Chunk::Vector& Chunk::get_size() const {
	return m_size;
}

Chunk::Block Chunk::get_block( const Vector& pos ) const {
	assert( pos.x < m_size.x || pos.y < m_size.y || pos.z < m_size.z );
	assert( m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] != INVALID_BLOCK );

	return m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x];
}

void Chunk::set_block( const Vector& pos, Block id ) {
	assert( pos.x < m_size.x || pos.y < m_size.y || pos.z < m_size.z );
	assert( id != INVALID_BLOCK );
	assert( id <= MAX_BLOCK_ID );

	m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] = id;
}

bool Chunk::is_block_set( const Vector& pos ) const {
	assert( pos.x < m_size.x && pos.y < m_size.y && pos.z < m_size.z );
	return m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] != INVALID_BLOCK;
}

void Chunk::reset_block( const Vector& pos ) {
	assert( pos.x < m_size.x || pos.y < m_size.y || pos.z < m_size.z );
	assert( m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] != INVALID_BLOCK );

	m_blocks[pos.z * (m_size.y * m_size.x) + pos.y * m_size.x + pos.x] = INVALID_BLOCK;
}

const Chunk::Block* Chunk::get_raw_data() const {
	return m_blocks;
}

}
