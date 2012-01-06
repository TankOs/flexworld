#include <FlexWorld/Socket.hpp>
#include <FlexWorld/Networking.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSocket ) {
	using namespace flex;

	Networking::init();

	// Initial state.
	{
		Socket socket;

		BOOST_CHECK( socket.is_connected() == false );
		BOOST_CHECK( socket.is_listening() == false );

		socket.close();
	}

	// Bind and listen (twice, to also check re-use of sockets).
	{
		Socket socket;

		BOOST_REQUIRE( socket.bind( "127.0.0.1", 2593 ) == true );
		BOOST_REQUIRE( socket.listen( 10 ) == true );
		BOOST_CHECK( socket.is_listening() == true );
		BOOST_CHECK( socket.is_connected() == false );

		socket.close();
		BOOST_CHECK( socket.is_listening() == false );
		BOOST_CHECK( socket.is_connected() == false );

		BOOST_CHECK( socket.bind( "127.0.0.1", 2593 ) == true );
		BOOST_CHECK( socket.listen( 10 ) == true );
		BOOST_CHECK( socket.is_listening() == true );
		BOOST_CHECK( socket.is_connected() == false );

		socket.close();
		BOOST_CHECK( socket.is_listening() == false );
		BOOST_CHECK( socket.is_connected() == false );
	}

	// Make a connection.
	{
		Socket listener;
		listener.bind( "127.0.0.1", 2593 );
		listener.listen( 10 );
		BOOST_REQUIRE( listener.is_listening() == true );

		Socket client;
		BOOST_CHECK( client.connect( "127.0.0.1", 2593 ) );
		BOOST_CHECK( client.is_connected() == true );
		BOOST_CHECK( client.is_listening() == false );

		Socket peer;
		BOOST_CHECK( listener.accept( peer ) == true );
		BOOST_CHECK( peer.is_connected() == true );
		BOOST_CHECK( peer.is_listening() == false );
		peer.close();

		client.close();
		BOOST_CHECK( client.is_connected() == false );
		BOOST_CHECK( client.is_listening() == false );

	}

	// Send and receive data.
	{
		Socket listener;
		listener.bind( "127.0.0.1", 2593 );
		listener.listen( 10 );
		BOOST_REQUIRE( listener.is_listening() == true );

		Socket client;
		client.connect( "127.0.0.1", 2593 );
		BOOST_REQUIRE( client.is_connected() == true );

		Socket peer;
		listener.accept( peer );
		BOOST_REQUIRE( peer.is_connected() == true );

		// Server -> Client.
		char buffer[6] = {0, 0, 0, 0, 0, 0};

		BOOST_CHECK( peer.send( "12345", 5 ) == 5 );
		BOOST_CHECK( client.receive( buffer, 5 ) == 5 );
		BOOST_CHECK( buffer == std::string( "12345" ) );

		// Client -> Server.
		BOOST_CHECK( client.send( "99999", 5 ) == 5 );
		BOOST_CHECK( peer.receive( buffer, 5 ) == 5 );
		BOOST_CHECK( buffer == std::string( "99999" ) );

		peer.close();
		client.close();
	}

	Networking::cleanup();
}
