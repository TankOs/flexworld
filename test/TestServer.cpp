#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <set>
#include <functional>
#include <iostream> // XXX

enum {
	WAIT_INTERVAL = 5,
	TIMEOUT = 5000
};

struct ServerHandler : flex::Server::Handler {
	ServerHandler() :
		num_logins_handled( 0 )
	{
	}

	void handle_connect( flex::Server::ConnectionID id ) {
		connected_clients.insert( id );
	}

	void handle_disconnect( flex::Server::ConnectionID id ) {
		connected_clients.erase( id );
	}

	void handle_message( const flex::msg::OpenLogin& msg, flex::Server::ConnectionID sender ) {
		BOOST_CHECK( connected_clients.find( sender ) != connected_clients.end() );
		BOOST_CHECK( msg.get_username() == "Tank" );
		BOOST_CHECK( msg.get_password() == "h4x0r" );
		BOOST_CHECK( msg.get_server_password() == "me0w" );
		++num_logins_handled;
	}

	std::set<flex::Server::ConnectionID> connected_clients;
	std::size_t num_logins_handled;
};

bool g_thread_running = false;

void run_server( flex::Server* server ) {
	g_thread_running = true;
	BOOST_REQUIRE( server->run() );
	g_thread_running = false;
}

std::shared_ptr<boost::thread> start_server_and_wait( flex::Server& server ) {
	g_thread_running = true;
	std::shared_ptr<boost::thread> run_thread(
		new boost::thread( std::bind( &run_server, &server ) )
	);

	// Wait for server to become ready.
	unsigned int time_passed = 0;

	while( time_passed < TIMEOUT && !server.is_running() && g_thread_running ) {
		boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
		time_passed += WAIT_INTERVAL;
	}

	BOOST_REQUIRE( server.is_running() );
	BOOST_REQUIRE( time_passed < TIMEOUT );

	return run_thread;
}

void connect_client_and_wait( boost::asio::ip::tcp::socket& client, flex::Server& server ) {
	std::size_t num_clients = server.get_num_peers();

	BOOST_CHECK_NO_THROW(
		client.connect(
			boost::asio::ip::tcp::endpoint(
				boost::asio::ip::address_v4::from_string( server.get_ip() ),
				server.get_port()
			)
		)
	);

	unsigned int time_passed = 0;

	while( time_passed < TIMEOUT && server.get_num_peers() == num_clients ) {
		boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
		time_passed += WAIT_INTERVAL;
	}

	BOOST_REQUIRE( time_passed < TIMEOUT );
	BOOST_REQUIRE( server.get_num_peers() == num_clients + 1 );
}

void wait_for_num_clients( std::size_t num, flex::Server& server ) {
	unsigned int time_passed = 0;

	while( time_passed < TIMEOUT && server.get_num_peers() != num ) {
		boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
		time_passed += WAIT_INTERVAL;
	}

	BOOST_REQUIRE( time_passed < TIMEOUT );
	BOOST_REQUIRE( server.get_num_peers() == num );
}

BOOST_AUTO_TEST_CASE( TestServer ) {
	using namespace flex;
	using namespace boost::asio;

	// Initial state.
	{
		ServerHandler handler;
		Server server( handler );

		BOOST_CHECK( server.get_ip() == "0.0.0.0" );
		BOOST_CHECK( server.get_port() == 2593 );
		BOOST_CHECK( server.get_num_peers() == 0 );
		BOOST_CHECK( server.is_running() == false );
	}

	// Basic properties.
	{
		ServerHandler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 1337 );

		BOOST_CHECK( server.get_ip() == "127.0.0.1" );
		BOOST_CHECK( server.get_port() == 1337 );
	}

	// Connect and disconnect a client.
	{
		ServerHandler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );

		std::shared_ptr<boost::thread> thread = start_server_and_wait( server );

		io_service service;
		ip::tcp::socket client( service );
		connect_client_and_wait( client, server );

		BOOST_CHECK( server.get_num_peers() == 1 );

		// Check IP.
		BOOST_CHECK( server.get_client_ip( 0 ) == "127.0.0.1" );

		// Disconnect client.
		server.disconnect_client( 0 );

		// Wait for server to signal he hasn't got clients anymore.
		{
			unsigned int time_passed = 0;

			while( time_passed < TIMEOUT && server.get_num_peers() > 0 ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			BOOST_REQUIRE( time_passed < TIMEOUT );
			BOOST_REQUIRE( server.get_num_peers() == 0 );
		}

		// Stop server.
		server.stop();
		thread->join();
	}

	// Connect client to get the following connected clients IDs:
	// 0, 1, 3
	// Do this by following these steps:
	// - Connect 0
	// - Connect 1
	// - Connect 2
	// - Connect 3
	// - Disconnect 2
	// - Disconnect 1
	// - Connect 1
	{
		ServerHandler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );

		std::shared_ptr<boost::thread> server_thread = start_server_and_wait( server );

		io_service service;
		ip::tcp::socket client0( service );
		ip::tcp::socket client1( service );
		ip::tcp::socket client2( service );
		ip::tcp::socket client3( service );

		connect_client_and_wait( client0, server );
		connect_client_and_wait( client1, server );
		connect_client_and_wait( client2, server );
		connect_client_and_wait( client3, server );

		BOOST_CHECK( handler.connected_clients.find( 0 ) != handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 1 ) != handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 2 ) != handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 3 ) != handler.connected_clients.end() );

		// Disconnect 2 and 1.
		client2.close();
		client1.close();

		wait_for_num_clients( 2, server );

		BOOST_CHECK( handler.connected_clients.find( 0 ) != handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 1 ) == handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 2 ) == handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 3 ) != handler.connected_clients.end() );

		// Connect 1.
		connect_client_and_wait( client1, server );

		wait_for_num_clients( 3, server );

		BOOST_CHECK( handler.connected_clients.find( 0 ) != handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 1 ) != handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 2 ) == handler.connected_clients.end() );
		BOOST_CHECK( handler.connected_clients.find( 3 ) != handler.connected_clients.end() );

		// Stop.
		client0.close();
		client1.close();
		client2.close();
		client3.close();

		server.stop();
		wait_for_num_clients( 0, server );
		server_thread->join();
	}

	// Connect client and send login message (client->server).
	{
		ServerHandler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );

		std::shared_ptr<boost::thread> thread = start_server_and_wait( server );

		// Connect.
		io_service service;
		ip::tcp::socket client( service );

		connect_client_and_wait( client, server );
		wait_for_num_clients( 1, server );

		// Send login message.
		msg::OpenLogin msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );
		msg.set_server_password( "me0w" );

		ServerProtocol::Buffer buf;
		ServerProtocol::serialize_message( msg, buf );

		// Spam the server with some messages.
		enum { NUM_MESSAGES = 1000 };
		enum { THIS_TIMEOUT = 10000 };

		for( std::size_t msg_id = 0; msg_id < NUM_MESSAGES; ++msg_id ) {
			client.send( buffer( &buf.front(), buf.size() ) );
		}

		// Wait until login msgs got handled.
		{
			unsigned int time_passed = 0;

			while( time_passed < THIS_TIMEOUT && handler.num_logins_handled != NUM_MESSAGES ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			BOOST_REQUIRE( time_passed < THIS_TIMEOUT );
			BOOST_REQUIRE( handler.num_logins_handled == NUM_MESSAGES );
		}

		client.close();
		server.stop();
		thread->join();
	}

	// Connect clients and send login messages (server->client).
	{
		ServerHandler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );

		std::shared_ptr<boost::thread> thread = start_server_and_wait( server );

		enum {
			NUM_CLIENTS = 20,
			NUM_MESSAGES_PER_CLIENT = 50
		};

		std::unique_ptr<ip::tcp::socket> client[NUM_CLIENTS];
		io_service service;

		// Connect all clients.
		for( std::size_t client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
			client[client_id].reset( new ip::tcp::socket( service ) );
			connect_client_and_wait( *client[client_id], server );
		}

		// Confirm.
		BOOST_REQUIRE( handler.connected_clients.size() == NUM_CLIENTS );

		// Now send messages to each client.
		msg::OpenLogin msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );
		msg.set_server_password( "me0w" );

		for( std::size_t msg_id = 0; msg_id < NUM_MESSAGES_PER_CLIENT; ++msg_id ) {
			for( std::size_t client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
				server.send_message( msg, static_cast<flex::Server::ConnectionID>( client_id ) );
			}
		}

		// Receive messages.
		for( std::size_t client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
			char buf[17];

			for( std::size_t msg_id = 0; msg_id < NUM_MESSAGES_PER_CLIENT; ++msg_id ) {
				std::size_t num_received = client[client_id]->receive( buffer( buf, 17 ) );

				BOOST_REQUIRE( num_received == 17 );
				BOOST_REQUIRE( buf[0] == 0 ); // Check msg ID.
				BOOST_REQUIRE( msg.deserialize( buf + 1, 16 ) == 16 ); // Skip msg id.

				BOOST_REQUIRE( msg.get_username() == "Tank" );
				BOOST_REQUIRE( msg.get_password() == "h4x0r" );
				BOOST_REQUIRE( msg.get_server_password() == "me0w" );
			}
		}

		// Disconnect all clients.
		for( std::size_t client_id = 0; client_id < NUM_CLIENTS; ++client_id ) {
			client[client_id]->close();
			client[client_id].reset();
		}

		server.stop();
		thread->join();
	}
}
