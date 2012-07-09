#include "Config.hpp"
#include "ExceptionChecker.hpp"

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
	using namespace fw;

	Log::Logger.set_min_level( Log::FATAL );

	GameMode game_mode;
	game_mode.set_default_entity_class_id( FlexID::make( "test/grass" ) );
	game_mode.add_package( FlexID::make( "test" ) );

	// Initial state.
	{
		LockFacility lock_facility;
		boost::asio::io_service service;
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
		LockFacility lock_facility;
		World world;

		// Create planet (for lock facility check, see below).
		world.create_planet( "foobar", Planet::Vector( 1, 1, 1 ), Chunk::Vector( 1, 1, 1 ) );

		const Planet* planet = world.find_planet( "foobar" );
		BOOST_REQUIRE( planet != nullptr );

		lock_facility.create_planet_lock( *planet );

		{
			boost::asio::io_service service;
			AccountManager acc_mgr;

			SessionHost host( service, lock_facility, acc_mgr, world, game_mode );
			host.set_ip( "127.0.0.1" );
			host.set_port( 2593 );

			// Add search path so host can find grass class.
			host.add_search_path( DATA_DIRECTORY + "/packages" );

			// Start host.
			BOOST_REQUIRE( host.start() );

			// Verify scripts got loaded.
			BOOST_CHECK( host.get_num_loaded_scripts() == 2 );

			// Check construct planet has been added to lock facility.
			BOOST_CHECK( world.get_num_planets() == 2 );
			BOOST_CHECK( lock_facility.get_num_planet_locks() == 2 );
		}

		// Check that SessionHost only removed planet locks of planets it created
		// itself. Also make sure planets are not removed at all.
		BOOST_CHECK( lock_facility.get_num_planet_locks() == 1 );
		BOOST_CHECK( world.get_num_planets() == 2 );

		lock_facility.destroy_planet_lock( *planet );
	}

	Log::Logger.set_min_level( Log::DEBUG );
}


class TestSessionHostGateClientHandler : public fw::Client::Handler {
	public:
		TestSessionHostGateClientHandler() :
			fw::Client::Handler(),
			m_num_chat_messages_received( 0 )
		{
		}

		void handle_message( const fw::msg::Chat& msg, fw::Server::ConnectionID /*conn_id*/ ) {
			m_last_chat_message = msg;
			++m_num_chat_messages_received;
		}

		void handle_message( const fw::msg::DestroyBlock& msg, fw::Server::ConnectionID /*conn_id*/ ) {
			m_last_destroy_block_message = msg;
		}

		void handle_message( const fw::msg::SetBlock& msg, fw::Server::ConnectionID /*conn_id*/ ) {
			m_last_set_block_message = msg;
		}

		void handle_message( const fw::msg::CreateEntity& msg, fw::Server::ConnectionID /*conn_id*/ ) {
			m_last_create_entity_message = msg;
		}

		void handle_message( const fw::msg::AttachEntity& msg, fw::Server::ConnectionID /*conn_id*/ ) {
			m_last_attach_entity_message = msg;
		}

		void handle_message( const fw::msg::ServerInfo& /*msg*/, fw::Server::ConnectionID /*conn_id*/ ) {}
		void handle_message( const fw::msg::LoginOK& /*msg*/, fw::Server::ConnectionID /*conn_id*/ ) {}
		void handle_connect( fw::Server::ConnectionID /*conn_id*/ ) {}
		void handle_disconnect( fw::Server::ConnectionID /*conn_id*/ ) {}

		std::size_t m_num_chat_messages_received;
		fw::msg::Chat m_last_chat_message;
		fw::msg::DestroyBlock m_last_destroy_block_message;
		fw::msg::SetBlock m_last_set_block_message;
		fw::msg::CreateEntity m_last_create_entity_message;
		fw::msg::AttachEntity m_last_attach_entity_message;
};

BOOST_AUTO_TEST_CASE( TestSessionHostGate ) {
	using namespace fw;

	Log::Logger.set_min_level( Log::FATAL );

	GameMode mode;
	AccountManager account_manager;

	mode.set_default_entity_class_id( FlexID::make( "fw.struct.simple/grass" ) );
	mode.add_package( FlexID::make( "sessionhostscripts" ) );

	enum { TIMEOUT = 2000 };

	// get_client_username, get_num_connected_clients, broadcast_chat_message, get_client_entity_id
	// (do tests here that need client connections)
	{
		// Setup host.
		boost::asio::io_service io_service;
		LockFacility lock_facility;
		World world;
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_player_limit( 2 );
		host.add_search_path( DATA_DIRECTORY + std::string( "/packages" ) );
		host.set_auth_mode( SessionHost::OPEN_AUTH );

		BOOST_REQUIRE( host.start() );

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
		std::string username;
		uint32_t entity_id = 123;

		BOOST_CHECK_NO_THROW( username = host.get_client_username( 0 ) );
		BOOST_CHECK_NO_THROW( entity_id = host.get_client_entity_id( 0 ) );

		BOOST_CHECK( username == "Tank" );
		BOOST_CHECK( entity_id == 0 );

		// Check invalid ID.
		BOOST_CHECK_EXCEPTION(
			host.get_client_username( 1 ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid client ID." )
		);

		BOOST_CHECK_EXCEPTION(
			host.get_client_entity_id( 1 ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid client ID." )
		);

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

		// Stop session host.
		host.stop();
		io_service.run();
	}

	// destroy_block
	{
		static const Planet::Vector CHUNK_POS( 0, 10, 0 );
		static const Chunk::Vector BLOCK_POS( 2, 3, 4 );
		static const std::string PLANET_ID = "construct";

		LockFacility lock_facility;

		// Create class.
		World world;

		{
			Class cls( FlexID::make( "a/class" ) );
			world.add_class( cls );
		}

		const Class *cls = world.find_class( FlexID::make( "a/class" ) );
		BOOST_REQUIRE( cls != nullptr );

		// Setup host.
		boost::asio::io_service io_service;
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_player_limit( 1 );
		host.add_search_path( DATA_DIRECTORY + std::string( "/packages" ) );
		host.set_auth_mode( SessionHost::OPEN_AUTH );

		BOOST_REQUIRE( host.start() );

		// Get planet.
		Planet* planet = world.find_planet( PLANET_ID );
		BOOST_REQUIRE( planet != nullptr );

		// Set a block.
		if( !planet->has_chunk( CHUNK_POS ) ) {
			planet->create_chunk( CHUNK_POS );
		}

		planet->set_block( CHUNK_POS, BLOCK_POS, *cls );

		BOOST_CHECK( planet->find_block( CHUNK_POS, BLOCK_POS ) == cls );

		// Connect client.
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

		// Destroy block.
		BOOST_CHECK_NO_THROW(
			host.destroy_block(
				lua::WorldGate::BlockPosition(
					CHUNK_POS.x * planet->get_chunk_size().x + BLOCK_POS.x,
					CHUNK_POS.y * planet->get_chunk_size().y + BLOCK_POS.y,
					CHUNK_POS.z * planet->get_chunk_size().z + BLOCK_POS.z
				),
			PLANET_ID )
		);
		BOOST_CHECK( planet->find_block( CHUNK_POS, BLOCK_POS ) == nullptr );

		// Poll IO service.
		io_service.poll();

		// Check that client received destroy block message.
		BOOST_CHECK(
			handler.m_last_destroy_block_message.get_block_position() == lua::WorldGate::BlockPosition(
				CHUNK_POS.x * planet->get_chunk_size().x + BLOCK_POS.x,
				CHUNK_POS.y * planet->get_chunk_size().y + BLOCK_POS.y,
				CHUNK_POS.z * planet->get_chunk_size().z + BLOCK_POS.z
			)
		);

		// Check invalid destroy calls.
		BOOST_CHECK_EXCEPTION( host.destroy_block( lua::WorldGate::BlockPosition( 0, 32, 0 ), PLANET_ID ), std::runtime_error, ExceptionChecker<std::runtime_error>( "No block at given position." ) );
		BOOST_CHECK_EXCEPTION( host.destroy_block( lua::WorldGate::BlockPosition( 0, 64, 0 ), PLANET_ID ), std::runtime_error, ExceptionChecker<std::runtime_error>( "No block at given position." ) );
		BOOST_CHECK_EXCEPTION( host.destroy_block( lua::WorldGate::BlockPosition( 99999, 99999, 99999 ), PLANET_ID ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Block position out of range." ) );
		BOOST_CHECK_EXCEPTION( host.destroy_block( lua::WorldGate::BlockPosition( 0, 0, 0 ), "foobar" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Planet not found." ) );
		BOOST_CHECK_EXCEPTION( host.destroy_block( lua::WorldGate::BlockPosition( 0, 0, 0 ), "" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid planet." ) );
	}

	// set_block
	{
		static const Planet::Vector CHUNK_POS( 0, 10, 0 );
		static const Chunk::Vector BLOCK_POS( 2, 3, 4 );
		static const std::string PLANET_ID = "construct";
		static const FlexID CLASS_ID = FlexID::make( "some/class" );

		// Create class.
		World world;

		{
			Class cls( CLASS_ID );
			world.add_class( cls );
		}

		const Class *cls = world.find_class( CLASS_ID );
		BOOST_REQUIRE( cls != nullptr );

		// Setup host.
		boost::asio::io_service io_service;
		LockFacility lock_facility;
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_player_limit( 1 );
		host.add_search_path( DATA_DIRECTORY + std::string( "/packages" ) );
		host.set_auth_mode( SessionHost::OPEN_AUTH );

		BOOST_REQUIRE( host.start() );

		// Get planet.
		Planet* planet = world.find_planet( PLANET_ID );
		BOOST_REQUIRE( planet != nullptr );

		// Make sure block doesn't exist yet.
		if( !planet->has_chunk( CHUNK_POS ) ) {
			planet->create_chunk( CHUNK_POS );
		}

		BOOST_CHECK( planet->find_block( CHUNK_POS, BLOCK_POS ) == nullptr );

		// Connect client.
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

		// Set block.
		BOOST_CHECK_NO_THROW(
			host.set_block(
				lua::WorldGate::BlockPosition(
					CHUNK_POS.x * planet->get_chunk_size().x + BLOCK_POS.x,
					CHUNK_POS.y * planet->get_chunk_size().y + BLOCK_POS.y,
					CHUNK_POS.z * planet->get_chunk_size().z + BLOCK_POS.z
				),
			PLANET_ID, CLASS_ID )
		);

		const Class* set_cls = planet->find_block( CHUNK_POS, BLOCK_POS );
		BOOST_CHECK( set_cls != nullptr );

		if( set_cls != nullptr ) {
			BOOST_CHECK( set_cls->get_id() == CLASS_ID );
		}

		// Poll IO service.
		io_service.poll();

		// Check that client received set block message.
		BOOST_CHECK(
			handler.m_last_set_block_message.get_block_position() == lua::WorldGate::BlockPosition(
				CHUNK_POS.x * planet->get_chunk_size().x + BLOCK_POS.x,
				CHUNK_POS.y * planet->get_chunk_size().y + BLOCK_POS.y,
				CHUNK_POS.z * planet->get_chunk_size().z + BLOCK_POS.z
			)
		);
		BOOST_CHECK( handler.m_last_set_block_message.get_class_id() == CLASS_ID.get() );

		// Check invalid set calls.
		BOOST_CHECK_EXCEPTION( host.set_block( lua::WorldGate::BlockPosition( 99999, 99999, 99999 ), PLANET_ID, CLASS_ID ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Block position out of range." ) );
		BOOST_CHECK_EXCEPTION( host.set_block( lua::WorldGate::BlockPosition( 0, 0, 0 ), "foobar", CLASS_ID ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Planet not found." ) );
		BOOST_CHECK_EXCEPTION( host.set_block( lua::WorldGate::BlockPosition( 0, 0, 0 ), "", CLASS_ID ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid planet." ) );
		BOOST_CHECK_EXCEPTION( host.set_block( lua::WorldGate::BlockPosition( 0, 0, 0 ), PLANET_ID, FlexID::make( "package" ) ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid class." ) );
		BOOST_CHECK_EXCEPTION( host.set_block( lua::WorldGate::BlockPosition( 0, 0, 0 ), PLANET_ID, FlexID::make( "no/exist" ) ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Class not found." ) );
	}

	// create_entity
	{
		static const FlexID CLASS_ID = FlexID::make( "some/class" );
		static const Planet::Coordinate ENTITY_POS( 1, 2, 3 );
		static const std::string PLANET_ID = "construct";

		// Create class.
		World world;

		{
			Class cls( CLASS_ID );
			world.add_class( cls );
		}

		const Class *cls = world.find_class( CLASS_ID );
		BOOST_REQUIRE( cls != nullptr );

		// Setup host.
		boost::asio::io_service io_service;
		LockFacility lock_facility;
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_player_limit( 1 );
		host.add_search_path( DATA_DIRECTORY + std::string( "/packages" ) );
		host.set_auth_mode( SessionHost::OPEN_AUTH );

		BOOST_REQUIRE( host.start() );

		// Get planet.
		Planet* planet = world.find_planet( PLANET_ID );
		BOOST_REQUIRE( planet != nullptr );

		// Make sure entity doesn't exist yet.
		BOOST_REQUIRE( world.find_entity( 0 ) == nullptr );

		// Connect client.
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

		// Create entity.
		BOOST_CHECK_NO_THROW(
			host.create_entity(
				CLASS_ID,
				ENTITY_POS,
				PLANET_ID
			)
		);

		// Verify.
		const Entity* entity = world.find_entity( 0 );

		BOOST_REQUIRE( entity != nullptr );
		BOOST_CHECK( entity->get_class().get_id() == CLASS_ID );
		BOOST_CHECK( entity->get_position() == ENTITY_POS );
		BOOST_CHECK( world.find_linked_planet( entity->get_id() )->get_id() == PLANET_ID );

		// Poll IO service.
		io_service.poll();

		// Check that client received message.
		BOOST_CHECK( handler.m_last_create_entity_message.get_id() == 0 );
		BOOST_CHECK( handler.m_last_create_entity_message.get_heading() == 0 );
		BOOST_CHECK( handler.m_last_create_entity_message.get_class() == CLASS_ID.get() );
		BOOST_CHECK( handler.m_last_create_entity_message.get_position() == ENTITY_POS );
		BOOST_CHECK( handler.m_last_create_entity_message.has_parent() == false );

		// Create entity attached to previously created entity.
		fw::Entity::ID attached_entity_id = 0;

		BOOST_CHECK_NO_THROW(
			attached_entity_id = host.create_entity(
				CLASS_ID,
				entity->get_id(),
				"inventory"
			)
		);

		// Verify.
		const fw::Entity* attached_entity = world.find_entity( attached_entity_id );

		BOOST_REQUIRE( attached_entity != nullptr );
		BOOST_CHECK( attached_entity->get_parent() == entity );
		BOOST_CHECK( entity->has_child( *attached_entity, "inventory" ) == true );

		// Poll IO service.
		io_service.poll();

		BOOST_CHECK( handler.m_last_create_entity_message.get_id() == attached_entity->get_id() );
		BOOST_CHECK( handler.m_last_create_entity_message.get_heading() == 0 );
		BOOST_CHECK( handler.m_last_create_entity_message.get_class() == CLASS_ID.get() );
		BOOST_CHECK( handler.m_last_create_entity_message.get_position() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( handler.m_last_create_entity_message.has_parent() == true );
		BOOST_CHECK( handler.m_last_create_entity_message.get_parent_hook() == "inventory" );
		BOOST_CHECK( handler.m_last_create_entity_message.get_parent_id() == entity->get_id() );

		// Create stowed entity.
		fw::Entity::ID stowed_entity_id = 0;

		BOOST_CHECK_NO_THROW(
			stowed_entity_id = host.create_entity(
				CLASS_ID,
				attached_entity->get_id()
			)
		);

		// Verify.
		const fw::Entity* stowed_entity = world.find_entity( stowed_entity_id );

		BOOST_REQUIRE( stowed_entity != nullptr );
		BOOST_CHECK( stowed_entity->get_parent() == attached_entity );
		BOOST_CHECK( attached_entity->has_child( *stowed_entity, "_cont" ) == true );

		// Poll IO service.
		io_service.poll();

		BOOST_CHECK( handler.m_last_create_entity_message.get_id() == stowed_entity->get_id() );
		BOOST_CHECK( handler.m_last_create_entity_message.get_heading() == 0 );
		BOOST_CHECK( handler.m_last_create_entity_message.get_class() == CLASS_ID.get() );
		BOOST_CHECK( handler.m_last_create_entity_message.get_position() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( handler.m_last_create_entity_message.has_parent() == true );
		BOOST_CHECK( handler.m_last_create_entity_message.get_parent_hook() == "_cont" );
		BOOST_CHECK( handler.m_last_create_entity_message.get_parent_id() == attached_entity->get_id() );


		// Check invalid calls.
		BOOST_CHECK_EXCEPTION(
			host.create_entity( FlexID::make( "package" ), ENTITY_POS, PLANET_ID ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid class ID." )
		);
		BOOST_CHECK_EXCEPTION(
			host.create_entity( FlexID::make( "no/exist" ), ENTITY_POS, PLANET_ID ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Class not found." )
		);

		BOOST_CHECK_EXCEPTION(
			host.create_entity( CLASS_ID, lua::WorldGate::EntityPosition( -1, 0, 0 ), PLANET_ID ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity position." )
		);
		BOOST_CHECK_EXCEPTION(
			host.create_entity( CLASS_ID, lua::WorldGate::EntityPosition( 0, -1, 0 ), PLANET_ID ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity position." )
		);
		BOOST_CHECK_EXCEPTION(
			host.create_entity( CLASS_ID, lua::WorldGate::EntityPosition( 0, 0, -1 ), PLANET_ID ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity position." )
		);

		BOOST_CHECK_EXCEPTION(
			host.create_entity(
				CLASS_ID,
				lua::WorldGate::EntityPosition(
					static_cast<float>( planet->get_size().x * planet->get_chunk_size().x ),
					0.0f,
					0.0f
				),
				PLANET_ID
			),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity position." )
		);
		BOOST_CHECK_EXCEPTION(
			host.create_entity(
				CLASS_ID,
				lua::WorldGate::EntityPosition(
					0.0f,
					static_cast<float>( planet->get_size().y * planet->get_chunk_size().y ),
					0.0f
				),
				PLANET_ID
			),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity position." )
		);
		BOOST_CHECK_EXCEPTION(
			host.create_entity(
				CLASS_ID,
				lua::WorldGate::EntityPosition(
					0.0f,
					0.0f,
					static_cast<float>( planet->get_size().z * planet->get_chunk_size().z )
				),
				PLANET_ID
			),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity position." )
		);

		BOOST_CHECK_EXCEPTION(
			host.create_entity( CLASS_ID, ENTITY_POS, "" ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid planet ID." )
		);
		BOOST_CHECK_EXCEPTION(
			host.create_entity( CLASS_ID, ENTITY_POS, "ficki" ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Planet not found." )
		);
	}

	// get_entity_position
	{
		// Setup host.
		boost::asio::io_service io_service;
		LockFacility lock_facility;
		World world;
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		// Create test resources.
		static const FlexID cls_id = FlexID::make( "some/class" );

		Class cls( cls_id );
		world.add_class( cls );
		world.create_planet( "foobar", Planet::Vector( 1, 1, 1 ), Chunk::Vector( 16, 16, 16 ) );

		lock_facility.create_planet_lock( *world.find_planet( "foobar" ) );

		Entity& entity = world.create_entity( cls_id );
		entity.set_position( sf::Vector3f( 1, 5, 10 ) );
		world.link_entity_to_planet( entity.get_id(), "foobar" );

		// Check.
		sf::Vector3f ent_pos( 0, 0, 0 );
		std::string planet_id;

		host.get_entity_position( entity.get_id(), ent_pos, planet_id );

		BOOST_CHECK( ent_pos == sf::Vector3f( 1, 5, 10 ) );
		BOOST_CHECK( planet_id == "foobar" );

		// Create entity without linking to planet.
		world.create_entity( cls_id );

		// Check invalid calls.
		BOOST_CHECK_EXCEPTION(
			host.get_entity_position( 100, ent_pos, planet_id ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Entity not found." )
		);

		BOOST_CHECK_EXCEPTION(
			host.get_entity_position( 1, ent_pos, planet_id ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Entity not linked to a planet." )
		);
	}

	// get_entity_class_id
	{
		// Setup host.
		boost::asio::io_service io_service;
		LockFacility lock_facility;
		World world;
		SessionHost host( io_service, lock_facility, account_manager, world, mode );

		// Create test resources.
		static const FlexID cls_id = FlexID::make( "some/class" );

		Class cls( cls_id );
		world.add_class( cls );
		world.create_planet( "foobar", Planet::Vector( 1, 1, 1 ), Chunk::Vector( 16, 16, 16 ) );

		lock_facility.create_planet_lock( *world.find_planet( "foobar" ) );

		Entity& entity = world.create_entity( cls_id );

		// Check.
		BOOST_CHECK( host.get_entity_class_id( entity.get_id() ) == "some/class" );

		// Check invalid calls.
		BOOST_CHECK_EXCEPTION(
			host.get_entity_class_id( 1337 ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Entity not found." )
		);
	}

	Log::Logger.set_min_level( Log::DEBUG );
}
