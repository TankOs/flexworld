#include <FlexWorld/Messages/Login.hpp>

#include <cstring>
#include <iostream> // XXX 

namespace flex {
namespace msg {

Login::Login() :
	Message()
{
}

void Login::serialize( Buffer& buffer ) const {
	// TODO: Check for correct password size (SHA-1 hash length?).
	// TODO: Check for valid characters?

	if( m_username.size() == 0 || m_username.size() > MAX_USERNAME_LENGTH ) {
		throw InvalidDataException( "Invalid username." );
	}

	if( m_password.size() == 0 ) {
		throw InvalidDataException( "Invalid password." );
	}

	std::size_t buf_ptr = buffer.size();

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( uint8_t ) // Username length.
		+ m_username.size() // Username.
		+ sizeof( uint8_t ) // Password length.
		+ m_password.size() // Password.
	);

	buffer[buf_ptr] = static_cast<uint8_t>( m_username.size() ); ++buf_ptr;
	std::memcpy( &buffer[buf_ptr], m_username.c_str(), m_username.size() ); buf_ptr += m_username.size();
	buffer[buf_ptr] = static_cast<uint8_t>( m_password.size() ); ++buf_ptr;
	std::memcpy( &buffer[buf_ptr], m_password.c_str(), m_password.size() ); buf_ptr += m_password.size();
}

std::size_t Login::deserialize( const Buffer& buffer ) {
	std::size_t buf_ptr( 0 );

	// Username length.
	if( buffer.size() - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t username_length = buffer[buf_ptr];
	buf_ptr += sizeof( uint8_t );

	if( username_length == 0 || username_length > MAX_USERNAME_LENGTH ) {
		throw BogusDataException( "Invalid username." );
	}

	// Check if username is present.
	if( buffer.size() - buf_ptr < username_length ) {
		return 0;
	}

	// Remember username ptr.
	const Buffer::value_type* username_ptr = &buffer[buf_ptr];
	buf_ptr += username_length;

	// Password.
	if( buffer.size() - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t password_length = buffer[buf_ptr];
	buf_ptr += sizeof( uint8_t );

	if( password_length == 0 ) {
		throw BogusDataException( "Invalid password." );
	}

	// Check if password present.
	if( buffer.size() - buf_ptr < password_length ) {
		return 0;
	}

	// Remember password ptr.
	const Buffer::value_type* password_ptr = &buffer[buf_ptr];
	buf_ptr += password_length;

	// Everything okay, set props.
	m_username = std::string( username_ptr, username_length );
	m_password = std::string( password_ptr, password_length );

	return buf_ptr;
}

void Login::set_username( const std::string& username ) {
	m_username = username;
}

const std::string& Login::get_username() const {
	return m_username;
}

void Login::set_password( const std::string& password ) {
	m_password = password;
}

const std::string& Login::get_password() const {
	return m_password;
}

}
}
