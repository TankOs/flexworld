#include <FlexWorld/Messages/Chunk.hpp>

#include <algorithm>
#include <cstring>
#include <cassert>

namespace flex {
namespace msg {

Chunk::Chunk() :
	Message(),
	m_position( 0, 0, 0 )
{
}

void Chunk::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	// Check required amount of blocks.
	if( m_blocks.size() == 0 ) {
		throw InvalidDataException( "Missing block data." );
	}

	if( m_blocks.size() > std::numeric_limits<NumBlocksType>::max() ) {
		throw InvalidDataException( "Too many blocks." );
	}

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( m_position ) // Position.
		+ sizeof( NumBlocksType ) // Number of blocks.
		+ sizeof( flex::Chunk::Block ) * m_blocks.size() // Block data.
	);

	// Position.
	*reinterpret_cast<Planet::Vector*>( &buffer[buf_ptr] ) = m_position;
	buf_ptr += sizeof( Planet::Vector );

	// Pack number of blocks.
	*reinterpret_cast<NumBlocksType*>( &buffer[buf_ptr] ) = static_cast<NumBlocksType>( m_blocks.size() );
	buf_ptr += sizeof( NumBlocksType );

	// Pack block data.
	for( std::size_t block_idx = 0; block_idx < m_blocks.size(); ++block_idx ) {
		*reinterpret_cast<flex::Chunk::Block*>( &buffer[buf_ptr] ) = m_blocks[block_idx];
		buf_ptr += sizeof( flex::Chunk::Block );
	}
}

std::size_t Chunk::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Position.
	if( buffer_size - buf_ptr < sizeof( m_position ) ) {
		return 0;
	}

	Planet::Vector position = *reinterpret_cast<const Planet::Vector*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( position );

	// Read number of blocks.
	if( buffer_size - buf_ptr < sizeof( NumBlocksType ) ) {
		return 0;
	}

	NumBlocksType num_blocks = *reinterpret_cast<const NumBlocksType*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( num_blocks );

	if( num_blocks == 0 ) {
		throw BogusDataException( "Invalid number of blocks." );
	}

	// Check if all blocks are present.
	if( buffer_size - buf_ptr < sizeof( flex::Chunk::Block ) * num_blocks ) {
		return 0;
	}

	// Read blocks.
	BlockVector blocks( num_blocks );

	for( std::size_t block_idx = 0; block_idx < num_blocks; ++block_idx ) {
		blocks[block_idx] = *reinterpret_cast<const flex::Chunk::Block*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( flex::Chunk::Block );

		// Check for valid ID.
		if( blocks[block_idx] > flex::Chunk::MAX_BLOCK_ID ) {
			throw BogusDataException( "Invalid block." );
		}
	}

	// Everything okay, store values.
	m_position = position;
	std::swap( m_blocks, blocks );

	return buf_ptr;
}

std::size_t Chunk::get_num_blocks() const {
	return m_blocks.size();
}

void Chunk::set_position( const Planet::Vector& pos ) {
	m_position = pos;
}

const Planet::Vector& Chunk::get_position() const {
	return m_position;
}

flex::Chunk::Block Chunk::get_block( std::size_t index ) const {
	assert( index < m_blocks.size() );
	return m_blocks[index];
}

void Chunk::set_blocks( const flex::Chunk& chunk ) {
	m_blocks.resize( chunk.get_size().x * chunk.get_size().y * chunk.get_size().z );

	std::memcpy(
		reinterpret_cast<char*>( &m_blocks[0] ),
		reinterpret_cast<const char*>( chunk.get_raw_data() ),
		sizeof( flex::Chunk::Block ) * m_blocks.size()
	);
}

}
}
