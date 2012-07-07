#include <FlexWorld/Messages/OpenContainer.hpp>

namespace flex {
namespace msg {

OpenContainer::OpenContainer() :
	m_id( 0 )
{
}

void OpenContainer::set_id( uint32_t id ) {
	m_id = id;
}

uint32_t OpenContainer::get_id() const {
	return m_id;
}

void OpenContainer::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buffer.size()
		+ sizeof( m_id )
	);

	*reinterpret_cast<uint32_t*>( &buffer[buf_ptr] ) = m_id;
	buf_ptr += sizeof( m_id );
}

std::size_t OpenContainer::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// ID.
	uint32_t id = 0;

	if( buffer_size - buf_ptr < sizeof( id ) ) {
		return 0;
	}

	id = *reinterpret_cast<const uint32_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( id );

	// Apply.
	m_id = id;

	return buf_ptr;
}

}
}
