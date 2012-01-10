#include "TestServerProtocolReactor.hpp"

#include <FlexWorld/Server.hpp>
#include <FlexWorld/ServerProtocol.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <functional>

void run_server( flex::Server* server ) {
	BOOST_REQUIRE( server->run() );
}

BOOST_AUTO_TEST_CASE( TestServer ) {
	using namespace flex;

	// Initial state.
	{
		TestServerProtocolReactor reactor;
		Server server( reactor );

		BOOST_CHECK( server.get_num_peers() == 0 );
		BOOST_CHECK( server.is_running() == false );
	}

	// Basic properties.
	{
		TestServerProtocolReactor reactor;
		Server server( reactor );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );
		server.set_num_dispatch_threads( 4 );

		BOOST_CHECK( server.get_ip() == "127.0.0.1" );
		BOOST_CHECK( server.get_port() == 2593 );
		BOOST_CHECK( server.get_num_dispatch_threads() == 4 );
	}

	static const uint32_t WAIT_INTERVAL = 250;

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

}
