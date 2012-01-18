#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <set>
#include <functional>

enum {
	WAIT_INTERVAL = 25,
	TIMEOUT = 2000
};

struct Handler : flex::Server::Handler {
	Handler() :
		login_handled( 0 )
	{
	}

	void handle_connect( flex::Server::ConnectionID id ) {
		connected_clients.insert( id );
	}

	void handle_disconnect( flex::Server::ConnectionID id ) {
		connected_clients.erase( id );
	}

	void handle_message( const flex::msg::Login& msg, flex::Server::ConnectionID sender ) {
		BOOST_CHECK( connected_clients.find( sender ) != connected_clients.end() );
		BOOST_CHECK( msg.get_username() == "Tank" );
		BOOST_CHECK( msg.get_password() == "h4x0r" );

		++login_handled;
	}

	std::set<flex::Server::ConnectionID> connected_clients;
	std::size_t login_handled;
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
	BOOST_CHECK_NO_THROW(
		client.connect(
			boost::asio::ip::tcp::endpoint(
				boost::asio::ip::address_v4::from_string( server.get_ip() ),
				server.get_port()
			)
		)
	);

	unsigned int time_passed = 0;
	std::size_t num_clients = server.get_num_peers();

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
		Handler handler;
		Server server( handler );

		BOOST_CHECK( server.get_ip() == "0.0.0.0" );
		BOOST_CHECK( server.get_port() == 2593 );
		BOOST_CHECK( server.get_num_peers() == 0 );
		BOOST_CHECK( server.is_running() == false );
	}

	// Basic properties.
	{
		Handler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 1337 );

		BOOST_CHECK( server.get_ip() == "127.0.0.1" );
		BOOST_CHECK( server.get_port() == 1337 );
	}

	// Connect and disconnect a client.
	{
		Handler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );

		std::shared_ptr<boost::thread> thread = start_server_and_wait( server );

		io_service service;
		ip::tcp::socket client( service );
		connect_client_and_wait( client, server );

		// Disconnect client.
		client.close();

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
		Handler handler;
		Server server( handler );

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
		server.stop();
		wait_for_num_clients( 0, server );
		server_thread->join();
	}

	// Connect client and send login message.
	{
		Handler handler;
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
		msg::Login msg;
		msg.set_username( "Tank" );
		msg.set_password( "h4x0r" );

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

			while( time_passed < THIS_TIMEOUT && handler.login_handled != NUM_MESSAGES ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			BOOST_REQUIRE( time_passed < THIS_TIMEOUT );
			BOOST_REQUIRE( handler.login_handled == NUM_MESSAGES );
		}

		server.stop();
		thread->join();
	}
}
