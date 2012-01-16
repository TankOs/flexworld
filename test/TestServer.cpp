#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <set>
#include <functional>

struct Handler : flex::Server::Handler {
	Handler() {
	}

	void handle_connect( flex::Server::ConnectionID id ) {
		connected_clients.insert( id );
	}

	void handle_disconnect( flex::Server::ConnectionID id ) {
		connected_clients.erase( id );
	}

	std::set<flex::Server::ConnectionID> connected_clients;
};

bool g_thread_running = false;

void run_server( flex::Server* server ) {
	g_thread_running = true;
	BOOST_REQUIRE( server->run() );
	g_thread_running = false;
}


BOOST_AUTO_TEST_CASE( TestServer ) {
	using namespace flex;

	// Initial state.
	{
		Handler handler;
		Server server( handler );

		BOOST_CHECK( server.get_num_peers() == 0 );
		BOOST_CHECK( server.is_running() == false );
		BOOST_CHECK( server.get_ip() == "0.0.0.0" );
		BOOST_CHECK( server.get_port() == 2593 );
		BOOST_CHECK( server.get_num_dispatch_threads() == 1 );
	}

	// Basic properties.
	{
		Handler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );
		server.set_num_dispatch_threads( 4 );

		BOOST_CHECK( server.get_ip() == "127.0.0.1" );
		BOOST_CHECK( server.get_port() == 2593 );
		BOOST_CHECK( server.get_num_dispatch_threads() == 4 );
	}

	static const uint32_t WAIT_INTERVAL = 50;
	static const uint32_t TIMEOUT = 2000;
	static const uint32_t NUM_CLIENTS = 10;

	// Connect some clients.
	{
		Handler handler;
		Server server( handler );
		server.set_ip( "127.0.0.1" );

		g_thread_running = true;
		boost::thread thread( std::bind( &run_server, &server ) );

		// Wait for server to come alive.
		while( g_thread_running && !server.is_running() ) {
			boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
		}

		BOOST_REQUIRE( server.is_running() );

		// Connect clients and wait until all are connected.
		flex::Socket socket[NUM_CLIENTS];

		for( Server::ConnectionID client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
			// Connect.
			BOOST_REQUIRE( socket[client_id].connect( server.get_ip(), server.get_port() ) == true );

			// Wait for connection being made.
			uint32_t time_passed = 0;

			while( time_passed < TIMEOUT && handler.connected_clients.find( client_id ) == handler.connected_clients.end() ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			// Check for timeout.
			BOOST_REQUIRE( time_passed < TIMEOUT );
		}

		// Disconnect clients and wait for server to terminate.
		for( Server::ConnectionID client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
			socket[client_id].shutdown();

			// Wait for connection being closed.
			uint32_t time_passed = 0;

			while( time_passed < TIMEOUT && handler.connected_clients.find( client_id ) != handler.connected_clients.end() ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			// Check for timeout.
			BOOST_REQUIRE( time_passed < TIMEOUT );
		}

		// At this point all connections have been closed, so terminate server and wait for it.
		server.shutdown();
		thread.join();
	}
}
/*

	// Run server and send messages from one peer.
	{
		TestServerProtocolReactor reactor;
		Server server( reactor );

		server.set_ip( "0.0.0.0" );
		server.set_port( 2593 );
		server.set_num_dispatch_threads( 4 );

		boost::thread thread( std::bind( &run_server, &server ) );

		// Wait for server to boot.
		while( !server.is_running() ) {
		}

		// Connect the peer and send some data.
		Socket client;
		BOOST_REQUIRE( client.connect( "127.0.0.1", 2593 ) );

		// Wait max. 5 seconds for connection to be established.
		unsigned int counter( 0 );
		while( server.get_num_peers() == 0 ) {
			if( counter >= 5000 / WAIT_INTERVAL ) {
				BOOST_REQUIRE( server.get_num_peers() == 1 );
				break;
			}

			boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
			++counter;
		}

		ServerProtocol::Buffer buffer;
		ServerProtocol::build_login_message( "Tank", "h4x0r", buffer );
		client.send( &buffer.front(), buffer.size() );

		while( server.is_running() ) {
			boost::this_thread::sleep( boost::posix_time::milliseconds( 200 ) );

			if( reactor.m_num_logins_received == 1 ) {
				break;
			}
		}

		std::cout << "*** GOING DOWN ***" << std::endl;

		server.shutdown();
		thread.join();
	}

}*/
