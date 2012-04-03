#include "Config.hpp"

#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>
#include <FlexWorld/GameMode.hpp>
#include <FlexWorld/Client.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Log.hpp>

#include <SFML/System/Clock.hpp>
#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSessionHost ) {
	using namespace flex;

	Log::Logger.set_min_level( Log::FATAL );

	GameMode game_mode;
	game_mode.set_default_entity_class_id( FlexID::make( "test/grass" ) );
	game_mode.add_package( FlexID::make( "test" ) );

	// Initial state.
	{
		boost::asio::io_service service;
		LockFacility lock_facility;
		AccountManager acc_mgr;
		World world;
		SessionHost host( service, lock_facility, acc_mgr, world, game_mode );

		BOOST_CHECK( &host.get_lock_facility() == &lock_facility );
		BOOST_CHECK( &host.get_account_manager() == &acc_mgr );
		BOOST_CHECK( &host.get_world() == &world );
		BOOST_CHECK( host.get_ip() == "0.0.0.0" );
		BOOST_CHECK( host.get_port() == 2593 );
		BOOST_CHECK( host.get_auth_mode() == SessionHost::OPEN_AUTH );
		BOOST_CHECK( host.get_player_limit() == 1 );
	}

	// Basic properties.
	{
		boost::asio::io_service service;
		LockFacility lock_facility;
		AccountManager acc_mgr;
		World world;
		SessionHost host( service, lock_facility, acc_mgr, world, game_mode );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_auth_mode( SessionHost::KEY_AUTH );
		host.set_player_limit( 128 );

		BOOST_CHECK( host.get_ip() == "127.0.0.1" );
		BOOST_CHECK( host.get_port() == 2593 );
		BOOST_CHECK( host.get_auth_mode() == SessionHost::KEY_AUTH );
		BOOST_CHECK( host.get_player_limit() == 128 );
	}

	enum { TIMEOUT = 2000 };
	enum { WAIT_INTERVAL = 25 };

	// Start and stop.
	{
		boost::asio::io_service service;
		LockFacility lock_facility;
		AccountManager acc_mgr;
		World world;

		SessionHost host( service, lock_facility, acc_mgr, world, game_mode );
		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );

		// Add search path so host can find grass class.
		host.add_search_path( DATA_DIRECTORY + "/packages" );

		// Start host.
		BOOST_REQUIRE( host.start() );

		// Verify scripts got loaded.
		BOOST_CHECK( host.get_num_loaded_scripts() == 2 );
	}

	Log::Logger.set_min_level( Log::DEBUG );
}


class TestSessionHostGateClientHandler : public flex::Client::Handler {
	public:
		TestSessionHostGateClientHandler() :
			flex::Client::Handler(),
			m_num_chat_messages_received( 0 )
		{
		}

		void handle_message( const flex::msg::Chat& msg, flex::Server::ConnectionID /*conn_id*/ ) {
			m_last_chat_message = msg;
			++m_num_chat_messages_received;
		}

		void handle_message( const flex::msg::ServerInfo& /*msg*/, flex::Server::ConnectionID /*conn_id*/ ) {}
		void handle_message( const flex::msg::LoginOK& /*msg*/, flex::Server::ConnectionID /*conn_id*/ ) {}
		void handle_connect( flex::Server::ConnectionID /*conn_id*/ ) {}
		void handle_disconnect( flex::Server::ConnectionID /*conn_id*/ ) {}

		std::size_t m_num_chat_messages_received;
		flex::msg::Chat m_last_chat_message;
};

BOOST_AUTO_TEST_CASE( TestSessionHostGate ) {
	using namespace flex;

	Log::Logger.set_min_level( Log::FATAL );

	boost::asio::io_service io_service;
	GameMode mode;
	World world;
	AccountManager account_manager;
	LockFacility lock_facility;

	mode.set_default_entity_class_id( FlexID::make( "fw.base.nature/grass" ) );
	mode.add_package( FlexID::make( "sessionhostscripts" ) );

	enum { TIMEOUT = 2000 };

	// Initial state.
	{
		// Setup host.
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_player_limit( 2 );
		host.add_search_path( DATA_DIRECTORY + std::string( "/packages" ) );
		host.set_auth_mode( SessionHost::OPEN_AUTH );

		BOOST_REQUIRE( host.start() );

		// get_client_username, get_num_connected_clients, broadcast_chat_message
		{
			TestSessionHostGateClientHandler handler;
			Client client( io_service, handler );

			client.start( host.get_ip(), host.get_port() );

			// Poll until client is connected.
			{
				sf::Clock timer;

				while( timer.getElapsedTime() < sf::milliseconds( TIMEOUT ) && host.get_num_connected_clients() != 1 ) {
					io_service.poll();
				}

				BOOST_REQUIRE( timer.getElapsedTime() < sf::milliseconds( TIMEOUT ) );
				BOOST_REQUIRE( host.get_num_connected_clients() == 1 );
			}

			// Authenticate.
			{
				msg::OpenLogin ol_msg;
				ol_msg.set_username( "Tank" );
				ol_msg.set_password( "h4x0r" );

				client.send_message( ol_msg );
			}

			// Poll IO service.
			io_service.poll();

			// At this point expected data should be ready.
			BOOST_CHECK_NO_THROW( host.get_client_username( 0 ) == "Tank" );

			// Check invalid ID.
			BOOST_CHECK_THROW( host.get_client_username( 1 ), std::runtime_error );

			// Connect another client to test broadcast_chat_message.
			TestSessionHostGateClientHandler handler2;
			Client client2( io_service, handler2 );

			client2.start( host.get_ip(), host.get_port() );

			// Poll until client is connected.
			{
				sf::Clock timer;

				while( timer.getElapsedTime() < sf::milliseconds( TIMEOUT ) && host.get_num_connected_clients() != 2 ) {
					io_service.poll();
				}

				BOOST_REQUIRE( timer.getElapsedTime() < sf::milliseconds( TIMEOUT ) );
				BOOST_REQUIRE( host.get_num_connected_clients() == 2 );
			}

			// Authenticate.
			{
				msg::OpenLogin ol_msg;
				ol_msg.set_username( "Tank2" );
				ol_msg.set_password( "h4x0r" );

				client.send_message( ol_msg );
				io_service.poll();
			}

			// Broadcast the message.
			std::size_t old_num_0 = handler.m_num_chat_messages_received;
			std::size_t old_num_1 = handler2.m_num_chat_messages_received;

			host.broadcast_chat_message( sf::String( "Meow" ), sf::String( "IAmA" ), sf::String( "Kitty" ) );

			{
				sf::Clock timer;

				while(
					timer.getElapsedTime() < sf::milliseconds( TIMEOUT ) &&
					(
						handler.m_num_chat_messages_received < (old_num_0 + 1) ||
						handler2.m_num_chat_messages_received < (old_num_1 + 1)
					)
				) {
					io_service.poll();
				}

				BOOST_REQUIRE( timer.getElapsedTime() < sf::milliseconds( TIMEOUT ) );
				BOOST_REQUIRE( handler.m_num_chat_messages_received == 1 );
				BOOST_REQUIRE( handler2.m_num_chat_messages_received == 1 );
			}

			BOOST_CHECK( handler.m_last_chat_message.get_message() == sf::String( "Meow" ) );
			BOOST_CHECK( handler.m_last_chat_message.get_channel() == sf::String( "IAmA" ) );
			BOOST_CHECK( handler.m_last_chat_message.get_sender() == sf::String( "Kitty" ) );
			BOOST_CHECK( handler2.m_last_chat_message.get_message() == sf::String( "Meow" ) );
			BOOST_CHECK( handler2.m_last_chat_message.get_channel() == sf::String( "IAmA" ) );
			BOOST_CHECK( handler2.m_last_chat_message.get_sender() == sf::String( "Kitty" ) );
		}

		// Stop session host.
		host.stop();
		io_service.run();
	}

	Log::Logger.set_min_level( Log::DEBUG );
}
