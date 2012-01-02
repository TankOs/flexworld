#include <FlexWorld/MessageMeta.hpp>

#include <cassert>
#include <stdexcept>

namespace flex {

MessageMeta::MessageMeta() :
	m_minimum_size( 0 )
{
}

MessageMeta::LengthType MessageMeta::get_field_size( Field field ) {
	assert( field >= BYTE && field <= STRING );

	switch( field ) {
		case BYTE:
			return static_cast<LengthType>( sizeof( ByteType ) );
			break;

		case WORD:
			return static_cast<LengthType>( sizeof( WordType ) );
			break;

		case DWORD:
			return static_cast<LengthType>( sizeof( DWordType ) );
			break;

		case STRING:
			return static_cast<LengthType>( sizeof( StringLengthType ) );
			break;

		default:
			break;
	}

	throw std::invalid_argument( "Invalid field type argument." );
}

std::size_t MessageMeta::get_num_fields() const {
	return m_fields.size();
}

MessageMeta::Field MessageMeta::get_field( std::size_t index ) const {
	assert( index < m_fields.size() );
	return m_fields[index];
}

void MessageMeta::add_field( Field type ) {
	assert( type <= STRING );

	m_minimum_size += get_field_size( type );
	m_fields.push_back( type );
}

uint32_t MessageMeta::get_minimum_size() const {
	return m_minimum_size;
}

}
