#include <FlexWorld/Client.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#define WAIT_WHILE( cond, var, interval, timeout ) {\
		unsigned int time_passed = 0; \
		while( time_passed < timeout && cond == var ) {\
			boost::this_thread::sleep( boost::posix_time::milliseconds( interval ) );\
			time_passed += interval;\
		}\
		BOOST_REQUIRE( time_passed < timeout );\
	}

struct ClientHandler : flex::Client::Handler {
	ClientHandler() :
		connected( false ),
		num_logins_handled( 0 )
	{
	}

	void handle_connect( flex::Client::ConnectionID id ) {
		BOOST_REQUIRE( id == 0 );
		connected = true;
	}

	void handle_disconnect( flex::Client::ConnectionID id ) {
		BOOST_REQUIRE( id == 0 );
		connected = false;
	}

	void handle_message( const flex::msg::Login& msg, flex::Client::ConnectionID /*id*/ ) {
		BOOST_CHECK( msg.get_username() == "Tank" );
		BOOST_CHECK( msg.get_password() == "h4x0r" );
		++num_logins_handled;
	}

	bool connected;
	std::size_t num_logins_handled;
};

void start_client( flex::Client* client ) {
	BOOST_REQUIRE( client->run( "127.0.0.1", 2593 ) );
}

void start_listener( boost::asio::ip::tcp::acceptor& listener ) {
	BOOST_CHECK_NO_THROW( listener.open( boost::asio::ip::tcp::v4() ) );
	BOOST_CHECK_NO_THROW( listener.set_option( boost::asio::socket_base::reuse_address( true ) ) );
	BOOST_CHECK_NO_THROW( listener.bind( boost::asio::ip::tcp::endpoint( boost::asio::ip::address_v4::from_string( "127.0.0.1" ), 2593 ) ) );
	BOOST_CHECK_NO_THROW( listener.listen() );
}

BOOST_AUTO_TEST_CASE( TestClient ) {
	using namespace flex;
	using namespace boost::asio;

	enum {
		WAIT_INTERVAL = 5,
		TIMEOUT = 3000
	};

	// Initial state.
	{
		ClientHandler handler;
		Client client( handler );

		BOOST_CHECK( client.is_connected() == false );
	}

	// Connect to server.
	{
		io_service service;
		ip::tcp::acceptor listener( service );
		start_listener( listener );

		// Connect.
		ClientHandler handler;
		Client client( handler );

		boost::thread thread( std::bind( &start_client, &client ) );

		// Accept client.
		ip::tcp::socket peer( service );
		listener.accept( peer );

		WAIT_WHILE( handler.connected, false, WAIT_INTERVAL, TIMEOUT );

		BOOST_REQUIRE( handler.connected == true );
		BOOST_REQUIRE( client.is_connected() );

		client.stop();
		thread.join();

		BOOST_REQUIRE( client.is_connected() == false );
	}

	// Send messages (server->client).
	{
		enum { NUM_MESSAGES = 100 };

		io_service service;
		ip::tcp::acceptor listener( service );
		start_listener( listener );

		// Connect.
		ClientHandler handler;
		Client client( handler );

		boost::thread thread( std::bind( &start_client, &client ) );

		// Accept client.
		ip::tcp::socket peer( service );
		listener.accept( peer );

		WAIT_WHILE( handler.connected, false, WAIT_INTERVAL, TIMEOUT );

		BOOST_REQUIRE( handler.connected == true );
		BOOST_REQUIRE( client.is_connected() );

		// Prepare message.
		msg::Login msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );

		ServerProtocol::Buffer buf;
		ServerProtocol::serialize_message( msg, buf );

		// Spam the client a little bit.
		for( std::size_t msg_id = 0; msg_id < NUM_MESSAGES; ++msg_id ) {
			peer.send( buffer( &buf[0], buf.size() ) );

			// Wait for msg to arrive.
			WAIT_WHILE( handler.num_logins_handled, msg_id, WAIT_INTERVAL, TIMEOUT );
			BOOST_REQUIRE( handler.num_logins_handled == msg_id + 1 );
		}

		listener.close();
		client.stop();
		thread.join();
	}

	// Send messages (client->server).
	{
		enum { NUM_MESSAGES = 100 };

		io_service service;
		ip::tcp::acceptor listener( service );
		start_listener( listener );

		// Connect.
		ClientHandler handler;
		Client client( handler );

		boost::thread thread( std::bind( &start_client, &client ) );

		// Accept client.
		ip::tcp::socket peer( service );
		listener.accept( peer );

		WAIT_WHILE( handler.connected, false, WAIT_INTERVAL, TIMEOUT );

		BOOST_REQUIRE( handler.connected == true );
		BOOST_REQUIRE( client.is_connected() );

		// Prepare message.
		msg::Login msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );

		// Send to server.
		client.send_message( msg );

		// Receive message at server.
		char buf[12];

		std::size_t num_received = peer.receive( buffer( buf, 12 ) );

		BOOST_REQUIRE( num_received == 12 );
		BOOST_REQUIRE( buf[0] == 0 ); // Msg ID.

		// Deserialize again to check more easily. ;-)
		msg.deserialize( buf + 1, 11 );
		BOOST_CHECK( msg.get_username() == "Tank" );
		BOOST_CHECK( msg.get_password() == "h4x0r" );

		listener.close();
		client.stop();
		thread.join();
	}
}
