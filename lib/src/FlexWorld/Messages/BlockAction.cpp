#include <FlexWorld/Messages/BlockAction.hpp>

namespace flex {
namespace msg {

BlockAction::BlockAction() :
	m_block_position( 0, 0, 0 ),
	m_facing( NORTH ),
	m_primary( true )
{
}

const BlockAction::BlockPosition& BlockAction::get_block_position() const {
	return m_block_position;
}

bool BlockAction::is_primary() const {
	return m_primary;
}

Facing BlockAction::get_facing() const {
	return m_facing;
}

void BlockAction::set_block_position( const BlockPosition& position ) {
	m_block_position = position;
}

void BlockAction::set_primary( bool primary ) {
	m_primary = primary;
}

void BlockAction::set_facing( Facing facing ) {
	m_facing = facing;
}

void BlockAction::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buf_ptr
		+ sizeof( BlockPosition )
		+ sizeof( uint8_t ) // Combined primary + facing (0xPF).
	);

	*reinterpret_cast<BlockPosition*>( &buffer[buf_ptr] ) = m_block_position;
	buf_ptr += sizeof( m_block_position );

	uint8_t combined = static_cast<uint8_t>( (m_primary << 4) | (m_facing & 0x0f) );
	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = combined;
	buf_ptr += sizeof( combined );
}

std::size_t BlockAction::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Block position.
	if( buffer_size - buf_ptr < sizeof( BlockPosition ) ) {
		return 0;
	}

	BlockPosition block_position = *reinterpret_cast<const BlockPosition*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( block_position );

	// Combined primary + facing.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t combined = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( combined );

	// Extract primary + facing.
	bool primary = static_cast<bool>( (combined >> 4) & 0x01 );
	int facing = static_cast<int>( combined & 0x0f );

	if( facing >= NUM_FACINGS ) {
		throw BogusDataException( "Invalid facing." );
	}

	// OK, apply.
	m_block_position = block_position;
	m_primary = primary;
	m_facing = static_cast<Facing>( facing );

	return buf_ptr;
}

}
}
