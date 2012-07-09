#include <FlexWorld/Messages/ServerInfo.hpp>

namespace fw {
namespace msg {

ServerInfo::ServerInfo() :
	m_auth_mode( static_cast<uint8_t>( OPEN_AUTH ) ),
	m_flags( static_cast<uint8_t>( NO_FLAGS ) )
{
}

void ServerInfo::serialize( Buffer& buffer ) const {
	if( m_auth_mode > KEY_AUTH ) {
		throw InvalidDataException( "Invalid auth mode." );
	}

	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buf_ptr
		+ sizeof( uint8_t )
		+ sizeof( uint8_t )
	);

	buffer[buf_ptr] = static_cast<uint8_t>( m_auth_mode ); buf_ptr += sizeof( uint8_t );
	buffer[buf_ptr] = static_cast<uint8_t>( m_flags ); buf_ptr += sizeof( uint8_t );
}

std::size_t ServerInfo::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Auth mode.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t auth_mode = buffer[buf_ptr];
	buf_ptr += sizeof( uint8_t );

	if( auth_mode > KEY_AUTH ) {
		throw BogusDataException( "Invalid auth mode." );
	}

	// Flags.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t flags = buffer[buf_ptr];
	buf_ptr += sizeof( uint8_t );

	m_auth_mode = auth_mode;
	m_flags = flags;

	return buf_ptr;
}

void ServerInfo::set_auth_mode( AuthMode mode ) {
	m_auth_mode = mode;
}

ServerInfo::AuthMode ServerInfo::get_auth_mode() const {
	return static_cast<AuthMode>( m_auth_mode );
}

void ServerInfo::set_flags( uint8_t flags ) {
	m_flags = flags;
}

uint8_t ServerInfo::get_flags() const {
	return m_flags;
}

}
}
