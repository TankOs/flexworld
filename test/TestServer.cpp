#include <FlexWorld/Server.hpp>

#include <boost/test/unit_test.hpp>
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

	// Connect a client.
	{
		Handler handler;
		Server server( handler );

		server.set_ip( "127.0.0.1" );
		server.set_port( 2593 );

		boost::thread run_thread( std::bind( &run_server, &server ) );

		run_thread.join();
	}
}
