#include <FlexWorld/Messages/DestroyBlock.hpp>

namespace flex {
namespace msg {

void DestroyBlock::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buffer.size()
		+ sizeof( m_block_position )
	);

	*reinterpret_cast<BlockPosition*>( &buffer[buf_ptr] ) = m_block_position;
	buf_ptr += sizeof( m_block_position );
}

std::size_t DestroyBlock::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Read block position.
	BlockPosition block_position;

	if( buffer_size - buf_ptr < sizeof( block_position ) ) {
		return 0;
	}

	block_position = *reinterpret_cast<const BlockPosition*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( block_position );

	// Apply.
	m_block_position = block_position;

	return buf_ptr;
}

DestroyBlock::DestroyBlock() :
	m_block_position( 0, 0, 0 )
{
}

const DestroyBlock::BlockPosition& DestroyBlock::get_block_position() const {
	return m_block_position;
}

void DestroyBlock::set_block_position( const BlockPosition& position ) {
	m_block_position = position;
}

}
}
