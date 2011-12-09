#include <FlexWorld/MessageMeta.hpp>

#include <stdexcept>

namespace flex {

std::size_t MessageMeta::get_type_size( FieldType type ) {
	switch( type ) {
		case BYTE: return sizeof( uint8_t );
		case WORD: return sizeof( uint16_t );
		case DWORD: return sizeof( uint32_t );
		case STRING: return sizeof( uint32_t );
		default:
			break;
	}

	throw std::out_of_range( "You're a bad programmer!" );
	return 0;
}

MessageMeta::MessageMeta() :
	m_minimum_size( 0 )
{
}

std::size_t MessageMeta::get_num_fields() const {
	return m_types.size();
}

MessageMeta::FieldType MessageMeta::get_field_type( std::size_t field ) const {
	return m_types.at( field );
}

void MessageMeta::add_field( FieldType type ) {
	m_minimum_size += static_cast<uint32_t>( get_type_size( type ) );
	m_types.push_back( type );
}

uint32_t MessageMeta::get_minimum_size() const {
	return m_minimum_size;
}

}
