#include <FlexWorld/Message.hpp>

#include <cassert>
#include <cstring>

// TODO: Endianness.

namespace flex {

Message::Message( const MessageMeta& meta ) :
	m_buffer( meta.get_minimum_size(), 0 ),
	m_indices( meta.get_num_fields(), 0 ),
	m_meta( meta )
{
	// Initialize indices.
	std::size_t offset( 0 );

	for( std::size_t field = 0; field < meta.get_num_fields(); ++field ) {
		m_indices[field] = offset;
		offset += MessageMeta::get_field_size( meta.get_field( field ) );
	}
}

MessageMeta::LengthType Message::get_size() const {
	return static_cast<MessageMeta::LengthType>( m_buffer.size() );
}

const MessageMeta& Message::get_meta() const {
	return m_meta;
}

MessageMeta::StringLengthType Message::get_string_length( std::size_t field ) const {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::STRING );
	
	return *reinterpret_cast<const MessageMeta::StringLengthType*>( &m_buffer[m_indices[field]] );
}

std::string Message::get_string( std::size_t field ) const {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::STRING );

	return std::string( reinterpret_cast<const char*>( &m_buffer.front() ) + m_indices[field] + sizeof( MessageMeta::StringLengthType ), get_string_length( field ) );
}

MessageMeta::DWordType Message::get_dword( std::size_t field ) const {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::DWORD );

	return *reinterpret_cast<const MessageMeta::DWordType*>( &m_buffer[m_indices[field]] );
}

MessageMeta::WordType Message::get_word( std::size_t field ) const {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::WORD );

	return *reinterpret_cast<const MessageMeta::WordType*>( &m_buffer[m_indices[field]] );
}

MessageMeta::ByteType Message::get_byte( std::size_t field ) const {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::BYTE );

	return *reinterpret_cast<const MessageMeta::ByteType*>( &m_buffer[m_indices[field]] );
}

void Message::set_string( std::size_t field, const std::string& string ) {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::STRING );

	std::size_t buffer_offset = m_indices[field];

	// Fetch old length.
	MessageMeta::StringLengthType old_length = static_cast<MessageMeta::StringLengthType>( *reinterpret_cast<MessageMeta::StringLengthType*>( &m_buffer[buffer_offset ] ) );
	MessageMeta::StringLengthType length = static_cast<MessageMeta::StringLengthType>( string.size() );

	// Check if we need to resize.
	if( length > old_length ) {
		// Need more bytes, enlarge buffer.
		std::size_t old_buffer_length( m_buffer.size() );
		m_buffer.resize( m_buffer.size() + (length - old_length) );

		// Move following data and updates indices.
		if( field + 1 < m_meta.get_num_fields() ) {
			std::memmove(
				&m_buffer[buffer_offset + sizeof( MessageMeta::StringLengthType ) + length],
				&m_buffer[buffer_offset + sizeof( MessageMeta::StringLengthType ) + old_length],
				old_buffer_length - buffer_offset - sizeof( MessageMeta::StringLengthType ) - old_length
			);

			for( std::size_t index_index = field + 1; index_index < m_indices.size(); ++index_index ) {
				m_indices[index_index] += (length - old_length);
			}
		}
	}
	else if( length < old_length ) {
		// Too many bytes, shrink buffer.
		std::size_t old_buffer_length( m_buffer.size() );
		m_buffer.resize( m_buffer.size() - (old_length - length) );

		// Move following data and updates indices.
		if( field + 1 < m_meta.get_num_fields() ) {
			std::memmove(
				&m_buffer[buffer_offset + sizeof( MessageMeta::StringLengthType ) + length],
				&m_buffer[buffer_offset + sizeof( MessageMeta::StringLengthType ) + old_length],
				old_buffer_length - buffer_offset - sizeof( MessageMeta::StringLengthType ) - old_length
			);

			for( std::size_t index_index = field + 1; index_index < m_indices.size(); ++index_index ) {
				m_indices[index_index] -= (old_length - length);
			}
		}
	}

	// Save new length.
	*reinterpret_cast<MessageMeta::StringLengthType*>( &m_buffer[buffer_offset] ) = length;

	// Save string.
	std::memcpy( &m_buffer[buffer_offset + sizeof( MessageMeta::StringLengthType )], string.c_str(), length );
}

void Message::set_dword( std::size_t field, MessageMeta::DWordType dword ) {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::DWORD );

	*reinterpret_cast<MessageMeta::DWordType*>( &m_buffer[m_indices[field]] ) = dword;
}

void Message::set_word( std::size_t field, MessageMeta::WordType word ) {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::WORD );

	*reinterpret_cast<MessageMeta::WordType*>( &m_buffer[m_indices[field]] ) = word;
}

void Message::set_byte( std::size_t field, MessageMeta::ByteType byte ) {
	assert( field < m_meta.get_num_fields() );
	assert( m_meta.get_field( field ) == MessageMeta::BYTE );

	*reinterpret_cast<MessageMeta::ByteType*>( &m_buffer[m_indices[field]] ) = byte;
}

}
