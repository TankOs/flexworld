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

std::size_t Selector::get_num_ready_sockets() const {
	return m_ready_sockets.size();
}

void Selector::add_socket( Socket& socket ) {
	assert( std::find( m_sockets.begin(), m_sockets.end(), &socket ) == m_sockets.end() );
	m_sockets.push_back( &socket );

	// Update fd_set and max socket value.
	FD_SET( socket.m_socket, &m_fd_set );
	m_max_socket = std::max( m_max_socket, socket.m_socket );
}

bool Selector::has_socket( Socket& socket ) const {
	return std::find( m_sockets.begin(), m_sockets.end(), &socket ) != m_sockets.end();
}

void Selector::remove_socket( Socket& socket ) {
	SocketVector::iterator iter = std::find( m_sockets.begin(), m_sockets.end(), &socket );
	assert( iter != m_sockets.end() );

	m_sockets.erase( iter );

	// Update fd_set and max socket value.
	FD_CLR( socket.m_socket, &m_fd_set );

	m_max_socket = 0;
	for( std::size_t socket_index = 0; socket_index < m_sockets.size(); ++socket_index ) {
		m_max_socket = std::max( m_max_socket, m_sockets[socket_index]->m_socket );
	}

	// Check if this socket has been marked as ready.
	SocketVector::iterator ready_iter = std::find( m_ready_sockets.begin(), m_ready_sockets.end(), &socket );

	if( ready_iter != m_ready_sockets.end() ) {
		m_ready_sockets.erase( ready_iter );
	}
}

void Selector::select( Mode mode, uint32_t timeout ) {
	assert( mode >= READ && mode <= WRITE );

	// Clear sets.
	m_ready_sockets.clear();

	// Cancel if no sockets were added.
	if( !get_num_sockets() ) {
		return;
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

	if( mode == WRITE ) {
		num_ready = ::select( m_max_socket + 1, nullptr, &m_fd_set, nullptr, (timeout != BLOCK ? &tv : nullptr) );
	}
	else if( mode == READ ) {
		num_ready = ::select( m_max_socket + 1, &m_fd_set, nullptr, nullptr, (timeout != BLOCK ? &tv : nullptr) );
	}
	else {
		assert( 0 && "Invalid mode." );
	}

	if( num_ready <= 0 ) {
		return;
	}

	for( std::size_t socket_index = 0; socket_index < m_sockets.size(); ++socket_index ) {
		if( FD_ISSET( m_sockets[socket_index]->m_socket, &m_fd_set ) ) {
			m_ready_sockets.push_back( m_sockets[socket_index] );
		}
	}
}

}
