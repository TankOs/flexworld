#include <FlexWorld/Server.hpp>
#include <FlexWorld/Selector.hpp>
#include <FlexWorld/Peer.hpp>

#include <boost/thread.hpp>
#include <cassert>

namespace flex {

Server::Server( Protocol& protocol ) :
	m_ip( "0.0.0.0" ),
	m_protocol( protocol ),
	m_num_dispatch_threads( 1 ),
	m_num_peers( 0 ),
	m_port( 2593 ),
	m_running( false )
{
}

Server::~Server() {
	shutdown();

	wait_for_threads();
	clean_threads();
	clean_sockets();
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

uint16_t Server::get_port() const{
	return m_port;
}

std::size_t Server::get_num_peers() const{
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
		if( m_peers[peer_id] == nullptr ) {
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

	m_running = true;

	// Setup the listener.
	m_listener.reset( new Socket );
	if( !m_listener->bind( m_ip, m_port ) || !m_listener->listen( 10 ) ) {
		m_running = false;
		return false;
	}

	// Listener is sane, launch dispatch threads.
	m_dispatch_threads.resize( m_num_dispatch_threads, nullptr );

	for( std::size_t thread_id = 0; thread_id < m_num_dispatch_threads; ++thread_id ) {
		m_dispatch_threads[thread_id] = new boost::thread( std::bind( &Server::run_dispatcher, this ) );
	}

	// Accept connections.
	Selector selector;
	selector.add_socket( *m_listener );

	while( m_running ) {
		// Wait for incoming connections and/or incoming data.
		selector.select( Selector::READ, Selector::BLOCK );

		// If select() returns without sockets ready, something bad happened.
		// Shutdown immediately.
		if( selector.get_num_ready_sockets() == 0 ) {
			break;
		}

		// Check for new incoming connection.
		if( selector.has_socket( *m_listener ) ) {
			// Create peer and accept connection.
			Peer* peer = new Peer;

			// If accepting fails then the listener has been shutdown.
			if( !m_listener->accept( peer->socket ) ) {
				// Remove from selector.
				selector.remove_socket( *m_listener );
				shutdown();

				delete peer;
			}
			else {
				// Accept it. Check if there's a free slot.
				std::size_t peer_id( 0 );

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
			}
		}
	}

	m_listener.reset();
	std::cout << "Cleanup complete, waiting for dispatcher threads..." << std::endl;
	wait_for_threads();
	clean_threads();

	m_running = false;

	return true;
}

void Server::clean_threads() {
	// WARNING: This doesn't check for ANY thread-safety!!! Be warned or lose me
	// forever.

	for( std::size_t thread_id = 0; thread_id < m_dispatch_threads.size(); ++thread_id ) {
		delete m_dispatch_threads[thread_id];
	}

	m_dispatch_threads.clear();
}

boost::mutex g_debug_mutex;

void Server::run_dispatcher() {
	g_debug_mutex.lock();
	static int id = 0;
	int local_id( id );
	++id;
	g_debug_mutex.unlock();

	boost::this_thread::sleep( boost::posix_time::seconds( 5 ) );
}

void Server::wait_for_threads() {
	for( std::size_t thread_id = 0; thread_id < m_dispatch_threads.size(); ++thread_id ) {
		m_dispatch_threads[thread_id]->join();
	}
}

void Server::clean_sockets() {
	for( std::size_t socket_index = 0; socket_index < m_peers.size(); ++socket_index ) {
		// Skip holes.
		if( m_peers[socket_index] == nullptr ) {
			continue;
		}

		// Make sure socket is closed and delete.
		m_peers[socket_index]->socket.close();
		delete m_peers[socket_index];
	}

	m_peers.clear();
}

}
