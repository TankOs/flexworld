#include <FlexWorld/Messages/ChunkUnchanged.hpp>

#include <cstring>

namespace fw {
namespace msg {

ChunkUnchanged::ChunkUnchanged() :
	Message(),
	m_position( 0, 0, 0 )
{
}

void ChunkUnchanged::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( m_position )
	);

	*reinterpret_cast<Planet::Vector*>( &buffer[buf_ptr] ) = m_position; buf_ptr += sizeof( Planet::Vector );
}

std::size_t ChunkUnchanged::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Position.
	if( buffer_size - buf_ptr < sizeof( m_position ) ) {
		return 0;
	}

	Planet::Vector position = *reinterpret_cast<const Planet::Vector*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( position );

	// Okay, apply.
	m_position = position;

	return buf_ptr;
}

void ChunkUnchanged::set_position( const Planet::Vector& pos ) {
	m_position = pos;
}

const Planet::Vector& ChunkUnchanged::get_position() const {
	return m_position;
}

}
}
