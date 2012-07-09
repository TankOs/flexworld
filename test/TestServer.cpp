#include <FlexWorld/Server.hpp>

#include <SFML/System/Clock.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <set>

class ServerHandler : public fw::Server::Handler {
	public:
		ServerHandler() :
			m_num_logins( 0 )
		{
		}

		void handle_connect( fw::Server::ConnectionID id ) {
			m_connected_clients.insert( id );
		}

		void handle_disconnect( fw::Server::ConnectionID id ) {
			m_connected_clients.erase( id );
		}

		void handle_message( const fw::msg::OpenLogin& msg, fw::Server::ConnectionID sender ) {
			BOOST_REQUIRE( msg.get_username() == "Tank" );
			BOOST_REQUIRE( msg.get_password() == "h4x0r" );
			BOOST_REQUIRE( msg.get_server_password() == "me0w" );

			++m_num_logins;

			if( m_num_client_logins.find( sender ) == m_num_client_logins.end() ) {
				m_num_client_logins[sender] = 1;
			}
			else {
				++m_num_client_logins[sender];
			}
		}

		const std::set<fw::Server::ConnectionID>& get_connected_clients() {
			return m_connected_clients;
		}

		std::size_t get_num_logins() {
			return m_num_logins;
		}

		std::size_t get_num_logins( fw::Server::ConnectionID client_id ) {
			return
				m_num_client_logins.find( client_id ) == m_num_client_logins.end() ?
				0 :
				m_num_client_logins[client_id]
			;
		}

	private:
		std::set<fw::Server::ConnectionID> m_connected_clients;
		std::map<fw::Server::ConnectionID, std::size_t> m_num_client_logins;
		std::size_t m_num_logins;
};

BOOST_AUTO_TEST_CASE( TestServer ) {
	using namespace fw;
	using namespace boost::asio;

	static const std::string IP = "127.0.0.1";
	static const unsigned short PORT = 1337;
	static const sf::Time TIMEOUT = sf::seconds( 3 );

	// Initial state.
	{
		io_service service;
		ServerHandler handler;
		Server server( service, handler );

		BOOST_CHECK( server.get_ip() == "0.0.0.0" );
		BOOST_CHECK( server.get_port() == 2593 );
		BOOST_CHECK( server.get_num_peers() == 0 );
		BOOST_CHECK( server.is_running() == false );
	}

	// Basic properties.
	{
		io_service service;
		ServerHandler handler;
		Server server( service, handler );

		server.set_ip( IP );
		server.set_port( PORT );

		BOOST_CHECK( server.get_ip() == IP );
		BOOST_CHECK( server.get_port() == PORT );
	}

	// Start server.
	{
		io_service service;
		ServerHandler handler;
		Server server( service, handler );

		server.set_ip( IP );
		server.set_port( PORT );

		BOOST_CHECK( server.start() == true );
		BOOST_CHECK( server.is_running() == true );

		// Starting it again will fail.
		BOOST_CHECK( server.start() == false );
		BOOST_CHECK( server.is_running() == true );
	}

	// Connect and disconnect a client.
	{
		io_service service;
		ServerHandler handler;

		Server server( service, handler );

		server.set_ip( IP );
		server.set_port( PORT );

		BOOST_REQUIRE( server.start() == true );
		BOOST_REQUIRE( server.is_running() == true );

		// Connect the client.
		ip::tcp::socket client( service );
		ip::tcp::endpoint endpoint( ip::address::from_string( IP ), PORT );

		client.connect( endpoint );

		// Wait for client to arrive.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.get_connected_clients().size() != 1 ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}
	}

	// Connect several clients. In the end the following client IDs are wanted:
	// 0, 1, 3
	// Reached by doing following steps:
	// - Connect 0
	// - Connect 1
	// - Connect 2
	// - Connect 3
	// - Disconnect 2
	// - Disconnect 1
	// - Connect 1
	{
		io_service service;
		ServerHandler handler;

		Server server( service, handler );

		server.set_ip( IP );
		server.set_port( PORT );

		BOOST_REQUIRE( server.start() == true );
		BOOST_REQUIRE( server.is_running() == true );

		// Prepare clients.
		ip::tcp::socket client0( service );
		ip::tcp::socket client1( service );
		ip::tcp::socket client2( service );
		ip::tcp::socket client3( service );

		// Connect 0-3.
		ip::tcp::endpoint endpoint( ip::address::from_string( IP ), PORT );

		client0.connect( endpoint );
		client1.connect( endpoint );
		client2.connect( endpoint );
		client3.connect( endpoint );

		// Wait for all clients to be connected.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.get_connected_clients().size() != 4 ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		// Disconnect 2 and 1 and wait for wanted state.
		client2.close();
		client1.close();

		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.get_connected_clients().size() != 2 ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		BOOST_REQUIRE( handler.get_connected_clients().find( 0 ) != handler.get_connected_clients().end() );
		BOOST_REQUIRE( handler.get_connected_clients().find( 1 ) == handler.get_connected_clients().end() );
		BOOST_REQUIRE( handler.get_connected_clients().find( 2 ) == handler.get_connected_clients().end() );
		BOOST_REQUIRE( handler.get_connected_clients().find( 3 ) != handler.get_connected_clients().end() );

		// Connect 1 and wait for desired state.
		client1.connect( endpoint );

		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.get_connected_clients().size() != 3 ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		BOOST_REQUIRE( handler.get_connected_clients().find( 0 ) != handler.get_connected_clients().end() );
		BOOST_REQUIRE( handler.get_connected_clients().find( 1 ) != handler.get_connected_clients().end() );
		BOOST_REQUIRE( handler.get_connected_clients().find( 2 ) == handler.get_connected_clients().end() );
		BOOST_REQUIRE( handler.get_connected_clients().find( 3 ) != handler.get_connected_clients().end() );
	}

	// Connect single client and send login message (client -> server).
	{
		io_service service;
		ServerHandler handler;

		Server server( service, handler );

		server.set_ip( IP );
		server.set_port( PORT );

		BOOST_REQUIRE( server.start() == true );
		BOOST_REQUIRE( server.is_running() == true );

		// Connect the client.
		ip::tcp::socket client( service );
		ip::tcp::endpoint endpoint( ip::address::from_string( IP ), PORT );

		client.connect( endpoint );

		// Wait for client to arrive.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.get_connected_clients().size() != 1 ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		// Send login message.
		msg::OpenLogin msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );
		msg.set_server_password( "me0w" );

		ServerProtocol::Buffer buf;
		ServerProtocol::serialize_message( msg, buf );

		// Spam the server with some messages.
		static const std::size_t NUM_MESSAGES = 1000;
		static const sf::Time MESSAGE_TIMEOUT = sf::seconds( 10 );

		for( std::size_t msg_id = 0; msg_id < NUM_MESSAGES; ++msg_id ) {
			client.send( buffer( &buf.front(), buf.size() ) );
		}

		// Wait until login msgs got handled.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < MESSAGE_TIMEOUT && handler.get_num_logins() != NUM_MESSAGES ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < MESSAGE_TIMEOUT );
		}
	}

	// Connnect multiple clients and send login message (server -> client).
	{
		enum { NUM_CLIENTS = 100 };
		enum { NUM_MESSAGES = 10 };

		io_service service;
		ServerHandler handler;

		Server server( service, handler );

		server.set_ip( IP );
		server.set_port( PORT );

		BOOST_REQUIRE( server.start() == true );
		BOOST_REQUIRE( server.is_running() == true );

		// Connect clients.
		std::unique_ptr<ip::tcp::socket> clients[NUM_CLIENTS];
		ip::tcp::endpoint endpoint( ip::address::from_string( IP ), PORT );

		for( std::size_t client_idx = 0; client_idx < NUM_CLIENTS; ++client_idx ) {
			clients[client_idx].reset( new ip::tcp::socket( service ) );
			clients[client_idx]->connect( endpoint );
		}

		// Wait for all clients to be connected.
		{
			sf::Clock timer;

			while( timer.getElapsedTime() < TIMEOUT && handler.get_connected_clients().size() != NUM_CLIENTS ) {
				service.poll();
			}

			BOOST_REQUIRE( timer.getElapsedTime() < TIMEOUT );
		}

		// Send messages to each client.
		msg::OpenLogin msg;
		msg.set_username( "Kitty" );
		msg.set_password( "Cat" );
		msg.set_server_password( "Meowz" );

		for( std::size_t client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
			for( std::size_t msg_idx = 0; msg_idx < NUM_MESSAGES; ++msg_idx ) {
				server.send_message( msg, static_cast<fw::Server::ConnectionID>( client_id ) );
			}
		}

		// Process all pending actions.
		while( service.poll() > 0 ) {
		}

		// Receive messages.
		char buf[17];
		std::size_t num_received = 0;

		for( std::size_t client_idx = 0; client_idx < NUM_CLIENTS; ++client_idx ) {
			clients[client_idx]->non_blocking( true );

			for( std::size_t msg_idx = 0; msg_idx < NUM_MESSAGES; ++msg_idx ) {
				num_received = clients[client_idx]->receive( buffer( buf, 17 ) );

				BOOST_REQUIRE( num_received == 17 );

				BOOST_REQUIRE( buf[0] == 0 ); // Message ID.
				BOOST_REQUIRE( msg.deserialize( buf + 1, 16 ) == 16 );

				BOOST_CHECK( msg.get_username() == "Kitty" );
				BOOST_CHECK( msg.get_password() == "Cat" );
				BOOST_CHECK( msg.get_server_password() == "Meowz" );
			}
		}
	}
}
