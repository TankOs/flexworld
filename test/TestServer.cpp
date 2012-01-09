#include "TestServerProtocolReactor.hpp"

#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <functional>

void run_server( flex::Server* server ) {
	BOOST_CHECK( server->run() );
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

	// Run server and send messages from one peer.
	{
		TestServerProtocolReactor reactor;
		Server server( reactor );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );
		server.set_num_dispatch_threads( 4 );

		boost::thread thread( std::bind( &run_server, &server ) );

		boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
		server.shutdown();

		thread.join();
	}

}
