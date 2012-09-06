#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWCS/Entity.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Reader.hpp>
#include <FWMS/Message.hpp>
#include <SFML/System/Time.hpp>
#include <boost/test/unit_test.hpp>

class EntityWatchdogReader : public ms::Reader {
	public:
		EntityWatchdogReader() :
			ms::Reader(),
			num_messages_received( 0 ),
			expected_fields( fw::ctrl::EntityWatchdog::UNCHANGED )
		{
		}

		void handle_message( const ms::Message& message ) {
			BOOST_REQUIRE( message.get_id() == ms::string_hash( "entity_change" ) );

			++num_messages_received;

			auto entity_id = message.find_property<fw::EntityID>( ms::string_hash( "id" ) );
			auto fields = message.find_property<int>( ms::string_hash( "fields" ) );
			auto snapshot = message.find_property<fw::ctrl::EntityWatchdog::Snapshot>( ms::string_hash( "snapshot" ) );

			BOOST_REQUIRE( entity_id != nullptr );
			BOOST_REQUIRE( fields != nullptr );
			BOOST_REQUIRE( snapshot != nullptr );

			BOOST_CHECK( snapshot->position == expected_position );
			BOOST_CHECK( *fields == expected_fields );
		}

		sf::Vector3f expected_position;
		std::size_t num_messages_received;
		int expected_fields;
};

void setup_entity( cs::Entity& entity ) {
	entity.create_property<bool>( "watch", true );
	entity.create_property<fw::EntityID>( "fw_entity_id", 1337 );
	entity.create_property<sf::Vector3f>( "position", sf::Vector3f( 1.0f, 2.0f, 3.0f ) );
}

BOOST_AUTO_TEST_CASE( TestEntityWatchdogController ) {
	using namespace fw;

	// Initial state.
	{
		ctrl::EntityWatchdog controller;

		BOOST_CHECK( controller.get_num_snapshots() == 0 );
		BOOST_CHECK( controller.get_router() == nullptr );
	}

	// Basic properties.
	{
		ms::Router router;

		ctrl::EntityWatchdog controller;
		controller.set_router( router );

		BOOST_CHECK( controller.get_router() == &router );
	}

	// Interesting entities.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		BOOST_CHECK( controller.is_entity_interesting( entity ) == true );
	}

	// Adding and removing creates first snapshot/removes snapshot. Message is
	// passed for first snapshot.
	{
		cs::Entity entity;
		setup_entity( entity );

		ms::Router router;

		ctrl::EntityWatchdog controller;
		controller.set_router( router );
		controller.add_entity( entity );

		BOOST_CHECK( controller.get_num_snapshots() == 1 );
		BOOST_CHECK( controller.find_snapshot( 1337 ) != nullptr );

		controller.remove_entity( entity );
		BOOST_CHECK( controller.get_num_snapshots() == 0 );
		BOOST_CHECK( controller.find_snapshot( 1337 ) == nullptr );
	}

	// First snapshot (after adding) reflects current entity's state and passes
	// message.
	{
		cs::Entity entity;
		setup_entity( entity );

		ms::Router router;
		EntityWatchdogReader& reader = router.create_reader<EntityWatchdogReader>();
		reader.expected_position = sf::Vector3f( 1.0f, 2.0f, 3.0f );
		reader.expected_fields = ctrl::EntityWatchdog::ALL;

		ctrl::EntityWatchdog controller;
		controller.set_router( router );
		controller.add_entity( entity );

		const auto snapshot = controller.find_snapshot( 1337 );
		BOOST_REQUIRE( snapshot != nullptr );

		snapshot->position == sf::Vector3f( 1.0f, 2.0f, 3.0f );

		router.process_queue();
		BOOST_CHECK( reader.num_messages_received == 1 );
	}

	// No changes do nothing.
	{
		cs::Entity entity;
		setup_entity( entity );

		ms::Router router;
		auto& reader = router.create_reader<EntityWatchdogReader>();
		reader.expected_position = sf::Vector3f( 1.0f, 2.0f, 3.0f );
		reader.expected_fields = ctrl::EntityWatchdog::ALL;

		ctrl::EntityWatchdog controller;
		controller.set_router( router );
		controller.add_entity( entity );

		ctrl::EntityWatchdog::Snapshot snapshot = *controller.find_snapshot( 1337 );
		controller.run( sf::milliseconds( 1 ) );
		BOOST_CHECK( *controller.find_snapshot( 1337 ) == snapshot );

		router.process_queue();
		BOOST_CHECK( reader.num_messages_received == 1 );
	}

	// Change position.
	{
		cs::Entity entity;
		setup_entity( entity );

		ms::Router router;

		ctrl::EntityWatchdog controller;
		controller.set_router( router );
		controller.add_entity( entity );

		// Process queue now to get rid of initial message.
		router.process_queue();

		auto& reader = router.create_reader<EntityWatchdogReader>();
		reader.expected_position = sf::Vector3f( 10.0f, 20.0f, 30.0f );
		reader.expected_fields = ctrl::EntityWatchdog::ALL;

		ctrl::EntityWatchdog::Snapshot snapshot = *controller.find_snapshot( 1337 );
		entity.find_property<sf::Vector3f>( "position" )->set_value( sf::Vector3f( 10.0f, 20.0f, 30.0f ) );

		controller.run( sf::milliseconds( 1 ) );

		BOOST_CHECK( (*controller.find_snapshot( 1337 ) == snapshot) == false );
		BOOST_CHECK( controller.find_snapshot( 1337 )->position.x == 10.0f );
		BOOST_CHECK( controller.find_snapshot( 1337 )->position.y == 20.0f );
		BOOST_CHECK( controller.find_snapshot( 1337 )->position.z == 30.0f );

		router.process_queue();
		BOOST_CHECK( reader.num_messages_received == 1 );
	}

	// ========================================

	// Snapshot equality.
	{
		ctrl::EntityWatchdog::Snapshot s0;
		ctrl::EntityWatchdog::Snapshot s1;

		BOOST_CHECK( s0 == s1 );

		s0 = s1;
		BOOST_CHECK( s0 == s1 );
		s0.position.x = 1.0f;
		BOOST_CHECK( !(s0 == s1) );

		s0 = s1;
		BOOST_CHECK( s0 == s1 );
		s0.position.y = 1.0f;
		BOOST_CHECK( !(s0 == s1) );

		s0 = s1;
		BOOST_CHECK( s0 == s1 );
		s0.position.z = 1.0f;
		BOOST_CHECK( !(s0 == s1) );
	}
}
