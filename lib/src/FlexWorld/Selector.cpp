#include <FlexWorld/Selector.hpp>

#include <algorithm>
#include <limits>
#include <cstring>
#include <cassert>

namespace flex {

const uint32_t Selector::POLL = 0;
const uint32_t Selector::BLOCK = std::numeric_limits<uint32_t>::max();

Selector::Selector() :
	m_max_socket( 0 )
{
	FD_ZERO( &m_fd_set );
}

std::size_t Selector::get_num_sockets() const {
	return m_sockets.size();
}

void Selector::add( Socket& socket ) {
	assert( std::find( m_sockets.begin(), m_sockets.end(), &socket ) == m_sockets.end() );
	m_sockets.push_back( &socket );

	// Update fd_set and max socket value.
	FD_SET( socket.m_socket, &m_fd_set );
	m_max_socket = std::max( m_max_socket, socket.m_socket );
}

bool Selector::has( Socket& socket ) const {
	return std::find( m_sockets.begin(), m_sockets.end(), &socket ) != m_sockets.end();
}

void Selector::remove( Socket& socket ) {
	SocketVector::iterator iter = std::find( m_sockets.begin(), m_sockets.end(), &socket );
	assert( iter != m_sockets.end() );

	m_sockets.erase( iter );

	// Update fd_set and max socket value.
	FD_CLR( socket.m_socket, &m_fd_set );
	FD_CLR( socket.m_socket, &m_ready_set );

	m_max_socket = 0;
	for( std::size_t socket_index = 0; socket_index < m_sockets.size(); ++socket_index ) {
		m_max_socket = std::max( m_max_socket, m_sockets[socket_index]->m_socket );
	}
}

std::size_t Selector::select( Mode mode, uint32_t timeout ) {
	assert( mode >= READ && mode <= WRITE );

	// Cancel if no sockets were added.
	if( !get_num_sockets() ) {
		return 0;
	}

	timeval tv;
	::memset( &tv, 0, sizeof( timeval ) );

	// Don't need to set anything when polling is wanted (sec = usec = 0 means
	// return immediately).
	if( timeout != POLL ) {
		tv.tv_sec = timeout / 1000000;
		tv.tv_usec = timeout % 1000000;
	}

	int num_ready( 0 );

	// Need to copy the fd_set because select() will modify it.
	std::memcpy( &m_ready_set, &m_fd_set, sizeof( fd_set ) );

	if( mode == WRITE ) {
		num_ready = ::select( m_max_socket + 1, nullptr, &m_ready_set, nullptr, (timeout != BLOCK ? &tv : nullptr) );
	}
	else if( mode == READ ) {
		num_ready = ::select( m_max_socket + 1, &m_ready_set, nullptr, nullptr, (timeout != BLOCK ? &tv : nullptr) );
	}
	else {
		assert( 0 && "Invalid mode." );
	}

	return num_ready < 0 ? 0 : static_cast<std::size_t>( num_ready );
}

bool Selector::is_ready( const Socket& socket ) const {
	assert( std::find( m_sockets.begin(), m_sockets.end(), &socket ) != m_sockets.end() );
	return FD_ISSET( socket.m_socket, &m_ready_set );
}

void Selector::clear() {
	m_sockets.clear();
	m_max_socket = 0;

	FD_ZERO( &m_fd_set );
}

}
