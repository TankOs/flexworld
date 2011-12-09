#include <FlexWorld/Message.hpp>

#include <stdexcept>
#include <limits>
#include <cstring>
#include <iostream> // XXX 

// TODO: Endianness.

namespace flex {

Message::Message( const MessageMeta& meta ) :
	m_buffer( meta.get_minimum_size(), 0 ),
	m_indices( meta.get_num_fields(), 0 ),
	m_meta( meta ),
	m_current_field( 0 )
{
	// Initialize indices.
	std::size_t offset( 0 );

	for( std::size_t field = 0; field < meta.get_num_fields(); ++field ) {
		m_indices[field] = offset;
		offset += meta.get_type_size( meta.get_field_type( field ) );
	}
}

std::size_t Message::get_size() const {
	return m_buffer.size();
}

std::size_t Message::get_field_offset( std::size_t field ) const {
	return m_indices.at( field );
}

Message& Message::operator<<( uint8_t value ) {
	if( !check_field( MessageMeta::BYTE ) ) {
		throw ValueTypeMismatch( "BYTE not expected." );
	}

	*reinterpret_cast<uint8_t*>( &m_buffer[m_indices[m_current_field]] ) = value;
	increase_field_pointer();

	return *this;
}

Message& Message::operator<<( int8_t value ) {
	if( !check_field( MessageMeta::BYTE ) ) {
		throw ValueTypeMismatch( "BYTE not expected." );
	}

	*reinterpret_cast<int8_t*>( &m_buffer[m_indices[m_current_field]] ) = value;
	increase_field_pointer();

	return *this;
}

Message& Message::operator<<( uint16_t value ) {
	if( !check_field( MessageMeta::WORD ) ) {
		throw ValueTypeMismatch( "WORD not expected." );
	}

	*reinterpret_cast<uint16_t*>( &m_buffer[m_indices[m_current_field]] ) = value;
	increase_field_pointer();

	return *this;
}

Message& Message::operator<<( int16_t value ) {
	if( !check_field( MessageMeta::WORD ) ) {
		throw ValueTypeMismatch( "WORD not expected." );
	}

	*reinterpret_cast<int16_t*>( &m_buffer[m_indices[m_current_field]] ) = value;
	increase_field_pointer();

	return *this;
}

Message& Message::operator<<( uint32_t value ) {
	if( !check_field( MessageMeta::DWORD ) ) {
		throw ValueTypeMismatch( "DWORD not expected." );
	}

	*reinterpret_cast<uint32_t*>( &m_buffer[m_indices[m_current_field]] ) = value;
	increase_field_pointer();

	return *this;
}

Message& Message::operator<<( int32_t value ) {
	if( !check_field( MessageMeta::DWORD ) ) {
		throw ValueTypeMismatch( "DWORD not expected." );
	}

	*reinterpret_cast<int32_t*>( &m_buffer[m_indices[m_current_field]] ) = value;
	increase_field_pointer();

	return *this;
}

Message& Message::operator<<( const std::string& value ) {
	if( !check_field( MessageMeta::STRING ) ) {
		throw ValueTypeMismatch( "STRING not expected." );
	}

	// Get old length first.
	uint32_t* length = reinterpret_cast<uint32_t*>( &m_buffer[m_indices[m_current_field]] );

	// Update indices and resize buffer.
	if( value.size() > *length ) {
		std::size_t diff( value.size() - *length );

		for( std::size_t field = m_current_field + 1; field < m_indices.size(); ++field ) {
			m_indices[field] += diff;
		}

		m_buffer.resize( m_buffer.size() + diff );
	}
	else if( value.size() < *length ) {
		std::size_t diff( *length - value.size() );

		for( std::size_t field = m_current_field + 1; field < m_indices.size(); ++field ) {
			m_indices[field] -= diff;
		}

		m_buffer.resize( m_buffer.size() - diff );
	}

	// Store string and update length.
	length = reinterpret_cast<uint32_t*>( &m_buffer[m_indices[m_current_field]] );
	*length = static_cast<uint32_t>( value.size() );
	std::memcpy( (&m_buffer[m_indices[m_current_field]]) + sizeof( uint32_t ), value.c_str(), *length );

	increase_field_pointer();
	return *this;
}

inline bool Message::check_field( MessageMeta::FieldType type ) {
	return m_current_field < m_meta.get_num_fields() && type == m_meta.get_field_type( m_current_field );
}

std::size_t Message::get_current_field() const {
	return m_current_field;
}

void Message::set_next_field( std::size_t field ) {
	if( field >= m_meta.get_num_fields() ) {
		throw std::out_of_range( "Field index is out of range." );
	}

	m_current_field = field;
}

inline void Message::increase_field_pointer() {
	if( m_current_field + 1 < m_meta.get_num_fields() ) {
		++m_current_field;
	}
}

}
