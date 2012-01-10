#include <FlexWorld/Selector.hpp>
#include <FlexWorld/Socket.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSelector ) {
	using namespace flex;

	// Initial state.
	{
		Selector selector;
		BOOST_CHECK( selector.get_num_sockets() == 0 );
	}

	// Add and remove sockets.
	{
		Socket socket[2];

		Selector selector;
		selector.add( socket[0] );
		selector.add( socket[1] );

		BOOST_CHECK( selector.has( socket[0] ) == true );
		BOOST_CHECK( selector.has( socket[1] ) == true );

		BOOST_CHECK( selector.get_num_sockets() == 2 );

		// Remove.
		selector.remove( socket[1] );
		BOOST_CHECK( selector.has( socket[0] ) == true );
		BOOST_CHECK( selector.has( socket[1] ) == false );

		BOOST_CHECK( selector.get_num_sockets() == 1 );

		selector.remove( socket[0] );
		BOOST_CHECK( selector.has( socket[0] ) == false );
		BOOST_CHECK( selector.has( socket[1] ) == false );

		BOOST_CHECK( selector.get_num_sockets() == 0 );
	}

	// Clear.
	{
		Socket socket[2];

		Selector selector;
		selector.add( socket[0] );
		selector.add( socket[1] );

		BOOST_CHECK( selector.get_num_sockets() == 2 );
		selector.clear();
		BOOST_CHECK( selector.get_num_sockets() == 0 );
	}

	// Select writable.
	{
		// At first with not connected socket.
		{
			Socket socket;

			Selector selector;
			selector.add( socket );

			BOOST_REQUIRE( selector.select( Selector::WRITE, 0 ) == 1 );

			// Even if the socket isn't connected a call to send() would not block
			// (it would return immediately with an error value).
			BOOST_CHECK( selector.is_ready( socket ) == true );
		}

		// Now with connected socket.
		{
			Socket listener;
			BOOST_REQUIRE( listener.bind( "127.0.0.1", 2593 ) );
			BOOST_REQUIRE( listener.listen( 10 ) == true );

			Socket client;
			BOOST_REQUIRE( client.connect( "127.0.0.1", 2593 ) == true );

			Selector selector;
			selector.add( client );
			BOOST_REQUIRE( selector.select( Selector::WRITE, 0 ) == 1 );
			BOOST_CHECK( selector.is_ready( client ) == true );
		}
	}

	// Select readable.
	{
		// At first with not connected socket.
		{
			Socket socket;

			Selector selector;
			selector.add( socket );

			BOOST_REQUIRE( selector.select( Selector::READ, 0 ) == 1 );

			// Even if the socket isn't connected a call to recv() would not block
			// (it would return immediately with an error value).
			BOOST_CHECK( selector.is_ready( socket ) == true );
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
			selector.add( client );
			BOOST_REQUIRE( selector.select( Selector::READ, 0 ) == 0 );
			BOOST_CHECK( selector.is_ready( client ) == false );
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
			selector.add( client );
			BOOST_REQUIRE( selector.select( Selector::READ, 0 ) == 1 );
			BOOST_CHECK( selector.is_ready( client ) == true );
		}
	}
}
