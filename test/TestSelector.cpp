#include <FlexWorld/Selector.hpp>
#include <FlexWorld/Socket.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSelector ) {
	using namespace flex;

	// Initial state.
	{
		Selector selector;

		BOOST_CHECK( selector.get_num_sockets() == 0 );
		BOOST_CHECK( selector.get_num_ready_sockets() == 0 );
	}

	// Add and remove sockets.
	{
		Socket socket[2];

		Selector selector;
		selector.add_socket( socket[0] );
		selector.add_socket( socket[1] );

		BOOST_CHECK( selector.has_socket( socket[0] ) == true );
		BOOST_CHECK( selector.has_socket( socket[1] ) == true );

		BOOST_CHECK( selector.get_num_sockets() == 2 );
		BOOST_CHECK( selector.get_num_ready_sockets() == 0 );

		// Remove.
		selector.remove_socket( socket[1] );
		BOOST_CHECK( selector.has_socket( socket[0] ) == true );
		BOOST_CHECK( selector.has_socket( socket[1] ) == false );

		BOOST_CHECK( selector.get_num_sockets() == 1 );
		BOOST_CHECK( selector.get_num_ready_sockets() == 0 );

		selector.remove_socket( socket[0] );
		BOOST_CHECK( selector.has_socket( socket[0] ) == false );
		BOOST_CHECK( selector.has_socket( socket[1] ) == false );

		BOOST_CHECK( selector.get_num_sockets() == 0 );
		BOOST_CHECK( selector.get_num_ready_sockets() == 0 );
	}

	// Select writable.
	{
		// At first with not connected socket.
		{
			Socket socket;

			Selector selector;
			selector.add_socket( socket );

			selector.select( Selector::WRITE, 0 );

			// Even if the socket isn't connected a call to send() would not block
			// (it would return immediately with an error value).
			BOOST_CHECK( selector.get_num_ready_sockets() == 1 );
		}

		// Now with connected socket.
		{
			Socket listener;
			listener.bind( "127.0.0.1", 2593 );
			BOOST_REQUIRE( listener.listen( 10 ) == true );

			Socket client;
			BOOST_REQUIRE( client.connect( "127.0.0.1", 2593 ) == true );

			Selector selector;
			selector.add_socket( client );
			selector.select( Selector::WRITE, 0 );

			BOOST_CHECK( selector.get_num_ready_sockets() == 1 );
		}
	}

	// Select readable.
	{
		// At first with not connected socket.
		{
			Socket socket;

			Selector selector;
			selector.add_socket( socket );

			selector.select( Selector::READ, 0 );

			// Even if the socket isn't connected a call to recv() would not block
			// (it would return immediately with an error value).
			BOOST_CHECK( selector.get_num_ready_sockets() == 1 );
		}

		// Now with connected socket but without pending data.
		{
			Socket listener;
			listener.bind( "127.0.0.1", 2593 );
			BOOST_REQUIRE( listener.listen( 10 ) == true );

			Socket client;
			BOOST_REQUIRE( client.connect( "127.0.0.1", 2593 ) == true );

			Socket peer;
			BOOST_REQUIRE( listener.accept( peer ) );

			Selector selector;
			selector.add_socket( client );
			selector.select( Selector::READ, 0 );

			BOOST_CHECK( selector.get_num_ready_sockets() == 0 );
		}

		// Now with connected socket and pending data.
		{
			Socket listener;
			listener.bind( "127.0.0.1", 2593 );
			BOOST_REQUIRE( listener.listen( 10 ) == true );

			Socket client;
			BOOST_REQUIRE( client.connect( "127.0.0.1", 2593 ) == true );

			Socket peer;
			BOOST_REQUIRE( listener.accept( peer ) );

			peer.send( "foo", 3 );

			Selector selector;
			selector.add_socket( client );
			selector.select( Selector::READ, 0 );

			BOOST_CHECK( selector.get_num_ready_sockets() == 1 );
		}
	}
}
