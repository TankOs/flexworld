#include <FlexWorld/ServerProtocol.hpp>

#include <iostream>
#include <cassert>

namespace flex {

ServerProtocol::ServerProtocol() :
	Protocol()
{
}

ServerProtocol::~ServerProtocol() {
}

void ServerProtocol::pack_string( const std::string& value, Buffer& buffer ) {
	buffer.resize( buffer.size() + value.size() );
	std::memcpy( &buffer[buffer.size() - value.size()], value.c_str(), value.size() );
}

std::size_t ServerProtocol::handle_incoming_data( Socket& socket, const Buffer& buffer ) {
	std::size_t handled( 0 );
	std::size_t min_required( 1 ); // Opcode.

	if( buffer.size() < min_required ) {
		return 0;
	}

	// Check for opcode.
	if( buffer[0] == LOGIN ) {
		++handled;

		// Read username size.
		min_required += 2;
		if( buffer.size() < min_required ) {
			return 0;
		}

		uint16_t username_size( 0 );
		username_size = *reinterpret_cast<const uint16_t*>( &buffer[handled] );
		handled += sizeof( uint16_t );

		if( username_size == 0 ) { // Bad guy is sending garbage?
			throw BogusMessageException( "Username size is zero." );
		}

		// Check for username string.
		min_required += username_size;
		if( buffer.size() < min_required ) {
			return 0;
		}

		const char* username_ptr( &buffer[handled] ); // For later use.
		handled += username_size;

		// Read password size.
		min_required += 2;
		if( buffer.size() < min_required ) {
			return 0;
		}

		uint16_t password_size( 0 );
		password_size = *reinterpret_cast<const uint16_t*>( &buffer[handled] );
		handled += sizeof( uint16_t );

		if( password_size == 0 ) { // Bad guy is sending garbage?
			throw BogusMessageException( "Password size is zero." );
		}

		// Check for password string.
		min_required += password_size;
		if( buffer.size() < min_required ) {
			return 0;
		}

		const char* password_ptr( &buffer[handled] );
		handled += password_size;

		// Message complete, dispatch.
		std::string username( username_ptr, username_size );
		std::string password( password_ptr, password_size );

		handle_login_message( socket, username, password );
	}

	return handled;
}

void ServerProtocol::build_login_message( const std::string& username, const std::string& password, Buffer& buffer ) {
	assert( !username.empty() );
	assert( !password.empty() );
	assert( username.size() < 256 );
	assert( password.size() < 256 );

	buffer.clear();

	pack_byte( static_cast<uint8_t>( LOGIN ), buffer );

	pack_word( static_cast<uint16_t>( username.size() ), buffer );
	pack_string( username, buffer );

	pack_word( static_cast<uint16_t>( password.size() ), buffer );
	pack_string( password, buffer );
}

void ServerProtocol::handle_login_message( Socket& /*sender*/, const std::string& /*username*/, const std::string& /*password*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Login message not handled." << std::endl;
	#endif
}

}
