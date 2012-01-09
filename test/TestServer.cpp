#include "TestServerProtocolReactor.hpp"

#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestServer ) {
	using namespace flex;

	TestServerProtocolReactor reactor;

	// Initial state.
	{
		Server server( reactor );

		BOOST_CHECK( server.get_num_clients() == 0 );
		BOOST_CHECK( server.is_listening() == false );
	}

	// Run server and send messages.
	{
		Server server( reactor );
		BOOST_REQUIRE( server.listen( "127.0.0.1", 2593 ) );

		// Connect a socket.
		Socket client;
		BOOST_REQUIRE( client.connect( "127.0.0.1", 2593 ) );

		// Process until connection is accepted.
		while( server.process() == 0 ) {
		}

		//Protocol::Buffer buffer;
		//ServerProtocol::build_login_message( "Tank", "foo", buffer );
	}

}
