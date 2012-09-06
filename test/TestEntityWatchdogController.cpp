#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWCS/Entity.hpp>
#include <SFML/System/Time.hpp>
#include <boost/test/unit_test.hpp>

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
		BOOST_CHECK( controller.get_callback_function() == nullptr );
	}

	// Basic properties.
	{
		ctrl::EntityWatchdog controller;

		auto callback = []( fw::EntityID, int ) {};
		controller.set_callback_function( callback );

		BOOST_CHECK( controller.get_callback_function() != nullptr );
	}

	// Interesting entities.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		BOOST_CHECK( controller.is_entity_interesting( entity ) == true );
	}

	// Adding and removing creates first snapshot/removes snapshot.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		controller.add_entity( entity );
		BOOST_CHECK( controller.get_num_snapshots() == 1 );
		BOOST_CHECK( controller.find_snapshot( 1337 ) != nullptr );

		controller.remove_entity( entity );
		BOOST_CHECK( controller.get_num_snapshots() == 0 );
		BOOST_CHECK( controller.find_snapshot( 1337 ) == nullptr );
	}

	// First snapshot (after adding) reflects current entity's state.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		controller.add_entity( entity );

		const auto snapshot = controller.find_snapshot( 1337 );
		BOOST_REQUIRE( snapshot != nullptr );

		snapshot->position == sf::Vector3f( 1.0f, 2.0f, 3.0f );
	}

	// No changes do nothing.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		controller.add_entity( entity );

		ctrl::EntityWatchdog::Snapshot snapshot = *controller.find_snapshot( 1337 );

		controller.run( sf::milliseconds( 1 ) );

		BOOST_CHECK( *controller.find_snapshot( 1337 ) == snapshot );
	}

	// Change position.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		controller.add_entity( entity );

		ctrl::EntityWatchdog::Snapshot snapshot = *controller.find_snapshot( 1337 );
		entity.find_property<sf::Vector3f>( "position" )->set_value( sf::Vector3f( 10.0f, 20.0f, 30.0f ) );

		controller.run( sf::milliseconds( 1 ) );

		BOOST_CHECK( (*controller.find_snapshot( 1337 ) == snapshot) == false );
		BOOST_CHECK( controller.find_snapshot( 1337 )->position.x == 10.0f );
		BOOST_CHECK( controller.find_snapshot( 1337 )->position.y == 20.0f );
		BOOST_CHECK( controller.find_snapshot( 1337 )->position.z == 30.0f );
	}

	// Callback.
	{
		cs::Entity entity;
		setup_entity( entity );

		ctrl::EntityWatchdog controller;
		std::size_t num_calls = 0;

		controller.set_callback_function(
			[&num_calls]( fw::EntityID entity_id, int changed_fields ) {
				BOOST_CHECK( entity_id == 1337 );
				BOOST_CHECK( changed_fields == ctrl::EntityWatchdog::POSITION );
				++num_calls;
			}
		);

		controller.add_entity( entity );
		entity.find_property<sf::Vector3f>( "position" )->set_value( sf::Vector3f( 10.0f, 20.0f, 30.0f ) );

		controller.run( sf::milliseconds( 1 ) );
		controller.run( sf::milliseconds( 1 ) );
		BOOST_CHECK( num_calls == 1 );

		entity.find_property<sf::Vector3f>( "position" )->set_value( sf::Vector3f( 20.0f, 30.0f, 40.0f ) );

		controller.run( sf::milliseconds( 1 ) );
		controller.run( sf::milliseconds( 1 ) );
		BOOST_CHECK( num_calls == 2 );
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
