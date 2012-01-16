#include <FlexWorld/Server.hpp>
#include <FlexWorld/Peer.hpp>

#include <boost/thread.hpp>
#include <iostream>
#include <cassert>

namespace flex {

/// HANDLER

void Server::Handler::handle_connect( ConnectionID id ) {
	std::cerr << "WARNING: Connection of #" << id << " not handled, closing." << std::endl;
	// TODO Close connection.
}

void Server::Handler::handle_disconnect( ConnectionID id ) {
	std::cerr << "WARNING: Disconnection of #" << id << " not handled." << std::endl;
}

// SERVER

Server::Server( Handler& handler ) :
	m_ip( "0.0.0.0" ),
	m_num_dispatch_threads( 1 ),
	m_handler( handler ),
	m_num_peers( 0 ),
	m_port( 2593 ),
	m_running( false )
{
}

Server::~Server() {
	shutdown();
	cleanup();
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

void Server::shutdown() {
	if( !m_running ) {
		return;
	}

	// Stop listener.
	m_listener->shutdown();

	// Shutdown all peers (gracefully).
	for( std::size_t peer_id = 0; peer_id < m_peers.size(); ++peer_id ) {
		// Skip free slots.
		if( !m_peers[peer_id] ) {
			continue;
		}

		m_peers[peer_id]->socket.shutdown();
	}
}

void Server::set_num_dispatch_threads( std::size_t num_threads ) {
	assert( !m_running );

	m_num_dispatch_threads = num_threads;
}

std::size_t Server::get_num_dispatch_threads() const {
	return m_num_dispatch_threads;
}

bool Server::run() {
	// Make sure server isn't already running.
	if( m_running ) {
		return false;
	}

	// Setup the listener.
	m_listener.reset( new Socket );
	if( !m_listener->bind( m_ip, m_port ) || !m_listener->listen( 10 ) ) {
		return false;
	}

	m_running = true;

	// Listener is sane, launch dispatch threads.
	m_dispatch_threads.resize( m_num_dispatch_threads, nullptr );

	for( std::size_t thread_id = 0; thread_id < m_num_dispatch_threads; ++thread_id ) {
		m_dispatch_threads[thread_id] = new boost::thread( std::bind( &Server::run_dispatcher, this ) );
	}

	// Prepare m_selector with listener.
	m_selector.add( *m_listener );

	while( m_running ) {
		// Wait for incoming connections and/or incoming data.
		std::size_t num_ready = m_selector.select( Selector::READ, Selector::BLOCK );

		// If select() returns without sockets ready, something bad happened.
		// Shutdown immediately.
		if( num_ready == 0 ) {
			break;
		}

		// Check if listener has something for me.
		if( m_selector.is_ready( *m_listener ) ) {
			process_listener();
			--num_ready;
		}

		// If still ready sockets left, a peer wants something.
		if( num_ready > 0 ) {
			process_peers();
		}
	}

	cleanup();

	m_running = false;
	return true;
}

void Server::notify_workers() {
	boost::lock_guard<boost::mutex> lock( m_worker_mutex );
	m_work_condition.notify_all();
}

void Server::cleanup() {
	// Wait for threads and delete them.
	m_running = false;
	notify_workers();

	for( std::size_t thread_id = 0; thread_id < m_dispatch_threads.size(); ++thread_id ) {
		m_dispatch_threads[thread_id]->join();
		delete m_dispatch_threads[thread_id];
	}

	m_dispatch_threads.clear();

	// Clean peers.
	m_peers.clear();

	// Delete listener.
	m_listener.reset();

	m_selector.clear();
	m_num_peers = 0;
}

void Server::run_dispatcher() {
	boost::unique_lock<boost::mutex> lock( m_worker_mutex );

	while( m_running ) {
		m_work_condition.wait( lock );

		{
			boost::lock_guard<boost::mutex> data_lock( m_work_data_mutex );
			continue;
		}
	}

}

void Server::process_listener() {
	// Create peer and accept connection.
	std::shared_ptr<Peer> peer( new Peer );

	// If accepting fails then the listener has been shutdown.
	if( !m_listener->accept( peer->socket ) ) {
		// Remove from m_selector and shutdown server.
		m_selector.remove( *m_listener );
		shutdown();

		return;
	}

	// Connection accepted.
	++m_num_peers;

	// Check if there's a free slot.
	ConnectionID peer_id( 0 );

	for( peer_id = 0; peer_id < m_peers.size(); ++peer_id ) {
		// If found, peer_id points to the correct index, so leave the loop.
		if( m_peers[peer_id] == nullptr ) {
			break;
		}
	}

	// If there's no free slot create a new one.
	if( peer_id >= m_peers.size() ) {
		m_peers.resize( peer_id + 1 );
	}

	// Store.
	m_peers[peer_id] = peer;

	// Add to m_selector and remember socket->ID mapping.
	m_selector.add( peer->socket );

	// Notify handler.
	m_handler.handle_connect( peer_id );
}

void Server::process_peers() {
	// Check all peers.
	std::size_t num_peers( m_peers.size() );

	for( ConnectionID peer_id = 0; peer_id < num_peers; ++peer_id ) {
		std::shared_ptr<Peer> peer = m_peers[peer_id];

		// Skip free slots.
		if( !peer ) {
			continue;
		}

		// Check if in selector.
		if( !m_selector.is_ready( peer->socket ) ) {
			continue;
		}

		// Receive data.
		static char buffer[1024];
		std::size_t num_bytes_received = peer->socket.receive( buffer, 1024 );

		// Check for disconnection.
		if( num_bytes_received == 0 ) {
			peer->socket.shutdown();
			peer->socket.close();

			// Notify handler.
			m_handler.handle_disconnect( peer_id );

			// If it's the last peer in the buffer, just shrink it.
			if( static_cast<std::size_t>( peer_id + 1 ) == m_peers.size() ) {
				m_peers.resize( m_peers.size() - 1 );
			}
			else { // Otherwise set the slot to NULL to indicate a free one.
				m_peers[peer_id] = std::shared_ptr<Peer>();
			}

			--m_num_peers;

			// Remove from m_selector.
			m_selector.remove( peer->socket );
		}
		else {
			// Data received, append to buffer.
			peer->buffer.resize( peer->buffer.size() + num_bytes_received );
			std::memcpy( (&peer->buffer[0]) + (peer->buffer.size() - num_bytes_received), buffer, num_bytes_received );

			// Call the protocol.
			//std::size_t bytes_processed = m_protocol.handle_incoming_data( peer_id, peer->buffer );

			// Shrink buffer.
			//if( bytes_processed > 0 ) {
				//if( bytes_processed >= peer->buffer.size() ) { // Kill whole buffer?
					//peer->buffer.clear();
				//}
				//else { // Move remaining data to front.
					//std::memmove( (&peer->buffer[0]) + bytes_processed, &peer->buffer[0], peer->buffer.size() - bytes_processed );
					//peer->buffer.resize( peer->buffer.size() - bytes_processed );
				//}
			//}
		}

	}
}

}
