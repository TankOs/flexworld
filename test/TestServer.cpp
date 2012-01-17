#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <set>
#include <functional>

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


BOOST_AUTO_TEST_CASE( TestServer ) {
	using namespace flex;

	static const unsigned int WAIT_INTERVAL = 25;
	static const unsigned int TIMEOUT = 2000;

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

		g_thread_running = true;
		boost::thread run_thread( std::bind( &run_server, &server ) );

		// Wait for server to become ready.
		{
			unsigned int time_passed = 0;

			while( time_passed < TIMEOUT && !server.is_running() && g_thread_running ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			BOOST_REQUIRE( server.is_running() );
			BOOST_REQUIRE( time_passed < TIMEOUT );
		}

		// Create socket and connect.
		using namespace boost::asio;

		io_service service;
		ip::tcp::socket client( service );
		BOOST_CHECK_NO_THROW( client.connect( ip::tcp::endpoint( ip::address_v4::from_string( server.get_ip() ), server.get_port() ) ) );

		// Wait for server to signal he's got 1 client.
		{
			unsigned int time_passed = 0;

			while( time_passed < TIMEOUT && server.get_num_peers() == 0 ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			BOOST_REQUIRE( time_passed < TIMEOUT );
			BOOST_REQUIRE( server.get_num_peers() == 1 );
		}

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
		run_thread.join();
	}
}
