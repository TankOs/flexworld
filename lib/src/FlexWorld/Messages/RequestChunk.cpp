#include <FlexWorld/Messages/RequestChunk.hpp>

#include <cstring>

namespace fw {
namespace msg {

RequestChunk::RequestChunk() :
	Message(),
	m_position( 0, 0, 0 ),
	m_timestamp( 0 )
{
}

void RequestChunk::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( m_position )
		+ sizeof( Timestamp )
	);

	*reinterpret_cast<Planet::Vector*>( &buffer[buf_ptr] ) = m_position; buf_ptr += sizeof( Planet::Vector );
	*reinterpret_cast<Timestamp*>( &buffer[buf_ptr] ) = m_timestamp; buf_ptr += sizeof( Timestamp );
}

std::size_t RequestChunk::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Position.
	if( buffer_size - buf_ptr < sizeof( m_position ) ) {
		return 0;
	}

	Planet::Vector position = *reinterpret_cast<const Planet::Vector*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( position );

	// Timestamp.
	if( buffer_size - buf_ptr < sizeof( m_timestamp ) ) {
		return 0;
	}

	Timestamp timestamp = *reinterpret_cast<const Timestamp*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( timestamp );

	// Okay, apply.
	m_position = position;
	m_timestamp = timestamp;

	return buf_ptr;
}

void RequestChunk::set_position( const Planet::Vector& pos ) {
	m_position = pos;
}

const Planet::Vector& RequestChunk::get_position() const {
	return m_position;
}

void RequestChunk::set_timestamp( Timestamp timestamp ) {
	m_timestamp = timestamp;
}

RequestChunk::Timestamp RequestChunk::get_timestamp() const {
	return m_timestamp;
}

}
}
