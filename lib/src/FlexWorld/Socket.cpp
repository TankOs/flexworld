#include <FlexWorld/Socket.hpp>
#include <FlexWorld/Networking.hpp>

#include <cstring>
#include <cassert>

namespace flex {

Socket::Socket() :
	m_socket( INVALID_SOCKET ),
	m_connected( false ),
	m_listening( false )
{
	m_socket = ::socket( AF_INET, SOCK_STREAM, 0 );
	assert( m_socket != INVALID_SOCKET );

	// TODO: REMOVE THIS AS WE'RE GRACEFULLY SHUTTING DOWN!
	int yep( 1 );
	int opt_result = setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>( &yep ), sizeof( yep ) );
	assert( opt_result == 0 );
}

Socket::~Socket() {
	close();
}

bool Socket::is_connected() const {
	return m_connected;
}

bool Socket::is_listening() const {
	return m_listening;
}

bool Socket::bind( const std::string& ip, uint16_t port ) {
	assert( !ip.empty() );
	assert( port > 0 );
	assert( !m_connected );
	assert( !m_listening );

	// Build socket address.
	::sockaddr_in addr;
	std::memset( &addr, 0, sizeof( ::sockaddr_in ) );

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ::inet_addr( ip.c_str() );
	if( addr.sin_addr.s_addr == INADDR_NONE ) {
		return false;
	}

	addr.sin_port = htons( port );

	return ::bind( m_socket, reinterpret_cast<sockaddr*>( &addr ), sizeof( sockaddr_in ) ) == 0;
}

bool Socket::listen( int backlog ) {
	assert( !m_connected );
	assert( !m_listening );

	int result = ::listen( m_socket, backlog );

	if( result == 0 ) {
		m_listening = true;
		return true;
	}

	return false;
}

bool Socket::connect( const std::string& ip, uint16_t port ) {
	assert( !m_connected );
	assert( !m_listening );
	assert( !ip.empty() );
	assert( port > 0 );

	// Build address.
	::sockaddr_in addr;
	std::memset( &addr, 0, sizeof( ::sockaddr_in ) );

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ::inet_addr( ip.c_str() );
	if( addr.sin_addr.s_addr == INADDR_NONE ) {
		return false;
	}
	addr.sin_port = htons( port );

	int result = ::connect( m_socket, reinterpret_cast<sockaddr*>( &addr ), sizeof( sockaddr_in ) );

	if( result == 0 ) {
		m_connected = true;
		return true;
	}

	return false;
}

void Socket::close() {
	if( m_connected ) {
		shutdown();
	}

	m_connected = false;
	m_listening = false;

	#if defined( LINUX )
		::close( m_socket );
	#else
		closesocket( m_socket );
	#endif
}

bool Socket::accept( Socket& peer ) {
	assert( m_listening );

	int result = ::accept( m_socket, nullptr, nullptr );

	if( result != -1 ) {
		// Make sure other peer is closed.
		peer.close();

		peer.m_socket = result;
		peer.m_connected = true;
		peer.m_listening = false;
		return true;
	}

	return false;
}

std::size_t Socket::send( const char* buffer, std::size_t size ) {
	assert( m_connected );
	assert( buffer != nullptr );
	assert( size > 0 );

	ssize_t sent_bytes = ::send( m_socket, buffer, size, 0 );

	if( sent_bytes < 1 ) {
		return 0;
	}

	return sent_bytes;
}

std::size_t Socket::receive( char* buffer, std::size_t size ) {
	assert( m_connected );
	assert( buffer != nullptr );
	assert( size > 0 );

	ssize_t received_bytes = ::recv( m_socket, buffer, size, 0 );

	if( received_bytes < 1 ) {
		return 0;
	}

	return received_bytes;
}

void Socket::shutdown() {
	assert( m_connected || m_listening );

	if( m_connected ) {
		::shutdown( m_socket, SHUT_WR );
	}
	else {
		::shutdown( m_socket, SHUT_RD );
	}
}

}
