#include <FlexWorld/Client.hpp>

#include <SFML/System/Clock.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>

struct ClientHandler : public fw::Client::Handler {
	ClientHandler() :
		connected( false ),
		num_logins_handled( 0 )
	{
	}

	void handle_connect( fw::Client::ConnectionID id ) {
		BOOST_REQUIRE( id == 0 );
		connected = true;
	}

	void handle_disconnect( fw::Client::ConnectionID id ) {
		BOOST_REQUIRE( id == 0 );
		connected = false;
	}

	void handle_message( const fw::msg::OpenLogin& msg, fw::Client::ConnectionID /*id*/ ) {
		BOOST_CHECK( msg.get_username() == "Tank" );
		BOOST_CHECK( msg.get_password() == "h4x0r" );
		BOOST_CHECK( msg.get_server_password() == "me0w" );
		++num_logins_handled;
	}

	bool connected;
	std::size_t num_logins_handled;
};

void start_listener( boost::asio::ip::tcp::acceptor& listener ) {
	BOOST_CHECK_NO_THROW( listener.open( boost::asio::ip::tcp::v4() ) );
	BOOST_CHECK_NO_THROW( listener.set_option( boost::asio::socket_base::reuse_address( true ) ) );
	BOOST_CHECK_NO_THROW( listener.bind( boost::asio::ip::tcp::endpoint( boost::asio::ip::address_v4::from_string( "127.0.0.1" ), 2593 ) ) );
	BOOST_CHECK_NO_THROW( listener.listen() );
}

BOOST_AUTO_TEST_CASE( TestClient ) {
	using namespace fw;
	using namespace boost::asio;

	static const std::string IP = "127.0.0.1";
	static const unsigned short PORT = 2593;
	static const sf::Time TIMEOUT = sf::seconds( 3 );

	// Initial state.
	{
		io_service service;
		ClientHandler handler;
		Client client( service, handler );

		BOOST_CHECK( client.is_started() == false );
	}

	// Basic properties.
	{
		io_service service;
		ClientHandler handler0;
		ClientHandler handler1;

		Client client( service, handler0 );
		BOOST_CHECK( &client.get_handler() == &handler0 );

		client.set_handler( handler1 );
		BOOST_CHECK( &client.get_handler() == &handler1 );
	}

	// Start.
	{
		io_service service;
		ClientHandler handler;
		Client client( service, handler );

		BOOST_CHECK( client.start( IP, PORT ) == true );
		BOOST_CHECK( client.is_started() == true );
	}

	// Connect to server.
	{
		io_service service;

		// Setup listener.
		ip::tcp::acceptor listener( service );
		start_listener( listener );

		// Connect.
		ClientHandler handler;
		Client client( service, handler );

		client.start( IP, PORT );

		// Accept client.
		ip::tcp::socket peer( service );
		listener.accept( peer );

		// Wait for connection.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.connected == false ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}
	}

	// Send messages (server->client).
	{
		enum { NUM_MESSAGES = 300 };

		io_service service;

		// Setup listener.
		ip::tcp::acceptor listener( service );
		start_listener( listener );

		// Connect.
		ClientHandler handler;
		Client client( service, handler );

		client.start( IP, PORT );

		// Accept client.
		ip::tcp::socket peer( service );
		listener.accept( peer );

		// Wait for connection.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.connected == false ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		// Prepare message.
		msg::OpenLogin msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );
		msg.set_server_password( "me0w" );

		ServerProtocol::Buffer buf;
		ServerProtocol::serialize_message( msg, buf );

		// Spam the client.
		for( std::size_t msg_idx = 0; msg_idx < NUM_MESSAGES; ++msg_idx ) {
			peer.send( buffer( &buf[0], buf.size() ) );

			// Wait for message to arrive.
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.num_logins_handled != msg_idx + 1 ) {
				service.poll();
			}
		}
	}

	// Send messages (server -> client).
	{
		enum { NUM_MESSAGES = 300 };

		io_service service;

		// Setup listener.
		ip::tcp::acceptor listener( service );
		start_listener( listener );

		// Connect.
		ClientHandler handler;
		Client client( service, handler );

		client.start( IP, PORT );

		// Accept client.
		ip::tcp::socket peer( service );
		listener.accept( peer );

		// Wait for connection.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.connected == false ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		// Prepare message.
		msg::OpenLogin msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );
		msg.set_server_password( "me0w" );

		// Send to server.
		client.send_message( msg );
		service.poll();

		// Receive message at server.
		char buf[17];

		std::size_t num_received = peer.receive( buffer( buf, 17 ) );

		BOOST_REQUIRE( num_received == 17 );
		BOOST_REQUIRE( buf[0] == 0 ); // Message ID.

		// Deserialize.
		msg.set_username( "foo" );
		msg.set_password( "foo" );
		msg.set_server_password( "foo" );
		msg.deserialize( buf + 1, 16 );

		BOOST_CHECK( msg.get_username() == "Tank" );
		BOOST_CHECK( msg.get_password() == "h4x0r" );
		BOOST_CHECK( msg.get_server_password() == "me0w" );
	}
}
