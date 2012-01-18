#include <FlexWorld/Server.hpp>
#include <FlexWorld/Peer.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <cassert>

namespace flex {

/// HANDLER

void Server::Handler::handle_connect( ConnectionID id ) {
	std::cerr << "WARNING: Connection of #" << id << " not handled!" << std::endl;
}

void Server::Handler::handle_disconnect( ConnectionID id ) {
	std::cerr << "WARNING: Disconnection of #" << id << " not handled!" << std::endl;
}

// SERVER

Server::Server( Handler& handler ) :
	m_ip( "0.0.0.0" ),
	m_handler( handler ),
	m_num_peers( 0 ),
	m_port( 2593 ),
	m_running( false )
{
}

Server::~Server() {
}

void Server::set_ip( const std::string& ip ) {
	assert( !ip.empty() );
	assert( !m_running );
	m_ip = ip;
}

const std::string& Server::get_ip() const {
	return m_ip;
}

void Server::set_port( uint16_t port ) {
	assert( !m_running );
	assert( port > 0 );
	m_port = port;
}

uint16_t Server::get_port() const {
	return m_port;
}

std::size_t Server::get_num_peers() const {
	return m_num_peers;
}

bool Server::is_running() const {
	return m_running;
}

bool Server::run() {
	namespace ip = boost::asio::ip;
	namespace asio = boost::asio;

	assert( !m_running );

	// Make sure server isn't already running.
	if( m_running ) {
		return false;
	}

	asio::io_service io_service;

	// Setup the listener.
	m_acceptor.reset( new ip::tcp::acceptor( io_service ) );
	m_acceptor->open( ip::tcp::v4() );
	m_acceptor->set_option( asio::socket_base::reuse_address( true ) );

	try {
		// Define local endpoint.
		ip::tcp::endpoint local_endpoint(
			ip::address_v4::from_string( get_ip() ), get_port()
		);

		// Bind address.
		m_acceptor->bind( local_endpoint );

		// Listen!
		m_acceptor->listen();

	}
	catch( const boost::system::system_error& e ) {
		m_acceptor.reset();
		return false;
	}

	m_running = true;

	// Accept connection.
	start_accept();

	// Run the service. This will block until all connections have been closed
	// and the listener died.
	io_service.run();

	// Cleanup.
	m_peers.clear();
	m_acceptor.reset();
	m_num_peers = 0;

	m_running = false;
	return true;
}

void Server::start_accept() {
	// Prepare the new peer.
	std::shared_ptr<Peer> peer( new Peer );
	peer->socket.reset( new boost::asio::ip::tcp::socket( m_acceptor->get_io_service() ) );

	m_acceptor->async_accept(
		*peer->socket,
		boost::bind( &Server::handle_accept, this, peer, boost::asio::placeholders::error )
	);
}

void Server::handle_accept( std::shared_ptr<Peer> peer, const boost::system::error_code& error ) {
	// If accepting failed, the listener has probably died.
	if( error ) {
		// Do not accept more connections, let the IO service die, in peace.
		return;
	}

	++m_num_peers;

	// Get next free connection ID.
	std::size_t conn_id = 0;

	for( conn_id = 0; conn_id < m_peers.size(); ++conn_id ) {
		// Found a free slot, use it.
		if( m_peers[conn_id] == nullptr ) {
			break;
		}
	}

	if( conn_id >= m_peers.size() ) {
		m_peers.push_back( peer );
	}
	else {
		m_peers[conn_id] = peer;
	}

	peer->id = static_cast<Peer::ConnectionID>( conn_id );

	// Notify observer.
	m_handler.handle_connect( peer->id );

	start_read( peer );
	start_accept();
}

void Server::start_read( std::shared_ptr<Peer> peer ) {
	peer->socket->async_read_some(
		boost::asio::buffer( peer->read_buffer, Peer::READ_BUFFER_SIZE ),
		boost::bind(
			&Server::handle_read,
			this,
			peer,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void Server::handle_read( std::shared_ptr<Peer> peer, const boost::system::error_code& error, std::size_t num_bytes_read ) {
	// Client disconnected?
	if( error ) {
		assert( peer->id < m_peers.size() );
		assert( m_peers[peer->id] == peer );

		if( static_cast<std::size_t>( peer->id + 1 ) == m_peers.size() ) {
			m_peers.pop_back();
		}
		else {
			m_peers[peer->id].reset();
		}

		--m_num_peers;
		m_handler.handle_disconnect( peer->id );

		return;
	}

	// Add to peer's buffer.
	peer->buffer.insert( peer->buffer.end(), peer->read_buffer, peer->read_buffer + num_bytes_read );

	// Try to dispatch.
	std::size_t consumed = 0;

	while( peer->buffer.size() && (consumed = ServerProtocol::dispatch( peer->buffer, m_handler, peer->id )) > 0 ) {
		if( consumed == peer->buffer.size() ) {
			peer->buffer.clear();
		}
		else {
			std::memmove( &peer->buffer[0], &peer->buffer[consumed], peer->buffer.size() - consumed );
			peer->buffer.resize( peer->buffer.size() - consumed );
		}
	}

	start_read( peer );
}

void Server::stop() {
	assert( m_running );

	// Do it cleanly be closing all sockets. The IO service will stop
	// automagically because there's no more work to do.
	for( std::size_t conn_id = 0; conn_id < m_peers.size(); ++conn_id ) {
		if( m_peers[conn_id] ) {
			m_peers[conn_id]->socket->close();
		}
	}

	m_acceptor->close();
}

void Server::handle_write( const boost::system::error_code& error, std::shared_ptr<ServerProtocol::Buffer> /*buffer*/, ConnectionID conn_id ) {
	// If failed to write, disconnect peer.
	if( error ) {
		std::cerr << "ERROR: Failed to send data to client #" << conn_id << ", disconnecting." << std::endl;

		if( conn_id < m_peers.size() && m_peers[conn_id] != nullptr ) {
			m_peers[conn_id]->socket->close();
		}

		return;
	}
}

}
