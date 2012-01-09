#include <FlexWorld/Server.hpp>

#include <cassert>

namespace flex {

Server::Server( Protocol& protocol ) :
	m_protocol( protocol ),
	m_listening( false )
{
}

Server::~Server() {
	shutdown();
}

bool Server::listen( const std::string& ip, uint16_t port ) {
	assert( !m_listening );

	m_listener.reset( new Socket );

	if( !m_listener->bind( ip, port ) ) {
		return false;
	}

	if( !m_listener->listen( 10 ) ) {
		return false;
	}

	m_listening = true;
	return true;
}

std::size_t Server::process() {
	assert( m_listening );

	std::size_t num_actions( 0 );

	return 1;
}

bool Server::is_listening() const {
	return m_listening;
}

std::size_t Server::get_num_clients() const {
	return 0;
}

void Server::shutdown() {
	if( !is_listening() && !get_num_clients() ) {
		return;
	}

	m_listener->close();
	m_listening = false;
}

}
