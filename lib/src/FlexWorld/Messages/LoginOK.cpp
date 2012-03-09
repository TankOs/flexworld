#include <FlexWorld/Messages/LoginOK.hpp>

#include <cstring>

namespace flex {
namespace msg {

LoginOK::LoginOK() :
	Message(),
	m_entity_id( 0 )
{
}

void LoginOK::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( m_entity_id ) // Entity ID.
	);

	*reinterpret_cast<Entity::ID*>( &buffer[buf_ptr] ) = m_entity_id; buf_ptr += sizeof( m_entity_id );
}

std::size_t LoginOK::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Entity ID.
	if( buffer_size - buf_ptr < sizeof( m_entity_id ) ) {
		return 0;
	}

	Entity::ID entity_id = *reinterpret_cast<const Entity::ID*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( entity_id );

	m_entity_id = entity_id;

	return buf_ptr;
}

void LoginOK::set_entity_id( Entity::ID id ) {
	m_entity_id = id;
}

Entity::ID LoginOK::get_entity_id() const {
	return m_entity_id;
}

}
}
