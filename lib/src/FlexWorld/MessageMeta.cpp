#include <FlexWorld/MessageMeta.hpp>

namespace flex {

MessageMeta::MessageMeta() {
}

std::size_t MessageMeta::get_field_count() const {
	return m_types.size();
}

MessageMeta::FieldType MessageMeta::get_field_type( std::size_t field ) const {
	if( field >= m_types.size() ) {
		return INVALID;
	}

	return m_types[field];
}

bool MessageMeta::add_field( FieldType type ) {
	if( type < INVALID || type >= _FIELD_TYPE_COUNT ) {
		return false;
	}

	m_types.push_back( type );
	return true;
}

}
