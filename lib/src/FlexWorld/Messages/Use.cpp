#include <FlexWorld/Messages/Use.hpp>

namespace flex {
namespace msg {

Use::Use() :
	m_entity_id( 0 )
{
}

void Use::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buf_ptr
		+ sizeof( m_entity_id )
	);

	*reinterpret_cast<uint32_t*>( &buffer[buf_ptr] ) = m_entity_id;
	buf_ptr += sizeof( m_entity_id );
}

std::size_t Use::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Entity ID.
	if( buffer_size - buf_ptr < sizeof( m_entity_id ) ) {
		return 0;
	}

	uint32_t entity_id = *reinterpret_cast<const uint32_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( m_entity_id );

	// Apply.
	m_entity_id = entity_id;

	return buf_ptr;
}

uint32_t Use::get_entity_id() const {
	return m_entity_id;
}

void Use::set_entity_id( uint32_t id ) {
	m_entity_id = id;
}

}
}
