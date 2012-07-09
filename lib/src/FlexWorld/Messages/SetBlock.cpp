#include <FlexWorld/Messages/SetBlock.hpp>

#include <cstring>

namespace fw {
namespace msg {

SetBlock::SetBlock() :
	m_block_position( 0, 0, 0 ),
	m_class_id( "" )
{
}

void SetBlock::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	if( m_class_id.empty() || m_class_id.size() > 255 ) {
		throw InvalidDataException( "Invalid class ID." );
	}

	buffer.resize(
		+ buf_ptr
		+ sizeof( m_block_position )
		+ sizeof( uint8_t ) // Class ID length.
		+ m_class_id.size()
	);

	*reinterpret_cast<BlockPosition*>( &buffer[buf_ptr] ) = m_block_position;
	buf_ptr += sizeof( m_block_position );

	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_class_id.size() );
	buf_ptr += sizeof( uint8_t );

	std::memcpy( &buffer[buf_ptr], m_class_id.c_str(), m_class_id.size() );
	buf_ptr += m_class_id.size();
}

std::size_t SetBlock::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Block position.
	if( buffer_size - buf_ptr < sizeof( BlockPosition ) ) {
		return 0;
	}

	BlockPosition block_position = *reinterpret_cast<const BlockPosition*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( block_position );

	// Class ID length.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t class_id_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( class_id_length );

	if( class_id_length < 1 ) {
		throw BogusDataException( "Invalid class ID length." );
	}

	// Class ID.
	if( buffer_size - buf_ptr < class_id_length ) {
		return 0;
	}

	const char* class_id_ptr = &buffer[buf_ptr];
	buf_ptr += class_id_length;

	// All OK, apply.
	m_block_position = block_position;
	m_class_id = std::string( class_id_ptr, class_id_length );

	return buf_ptr;
}

const SetBlock::BlockPosition& SetBlock::get_block_position() const {
	return m_block_position;
}

const std::string& SetBlock::get_class_id() const {
	return m_class_id;
}

void SetBlock::set_block_position( const BlockPosition& block_position ) {
	m_block_position = block_position;
}

void SetBlock::set_class_id( const std::string& class_id ) {
	m_class_id = class_id;
}

}
}
