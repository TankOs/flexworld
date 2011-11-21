#include <FlexWorld/Server.hpp>

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <iostream> // XXX 

namespace flex {

Server::Server() :
	m_run( false ),
	m_auth_mode( OPEN_MODE )
{
}

void Server::set_auth_mode( AuthMode mode ) {
	m_auth_mode = mode;
}

bool Server::listen( const std::string& /*ip*/, uint16_t port ) {
	// Shutdown server if it's already running.
	if( m_run ) {
		shutdown();
	}

	// TODO: Specify IP; not possible with SFML currently.
	m_tcp_listener.SetBlocking( true );
	m_tcp_listener.Listen( port );

	// Setup selector.
	sf::SocketSelector selector;
	selector.Add( m_tcp_listener );

	while( m_run ) {
		if( !selector.Wait() ) {
			// No selector ready without timeout? Something went wrong.
			shutdown();
			break;
		}

		// Check for incoming connection.
		if( selector.IsReady( m_tcp_listener ) ) {
			sf::TcpSocket peer;
			m_tcp_listener.Accept( peer );

			std::cout << "Incoming connection from " << peer.GetRemoteAddress() << std::endl;
			peer.Disconnect();
		}
	}

	return true;
}

void Server::shutdown() {
	// Skip shutdown if not running.
	if( !m_run ) {
		return;
	}

	m_tcp_listener.Close();
	m_run = false;
}

}
