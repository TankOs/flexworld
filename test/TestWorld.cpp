#include <FlexWorld/World.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/FlexID.hpp>
#include <FlexWorld/Class.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestWorld ) {
	using namespace flex;

	// Initial state.
	{
		World world;

		BOOST_CHECK( world.get_num_planets() == 0 );
		BOOST_CHECK( world.get_num_entities() == 0 );
		BOOST_CHECK( world.get_num_classes() == 0 );
	}

	// Create planets.
	{
		World world;

		BOOST_CHECK( world.get_num_planets() == 0 );
		BOOST_CHECK( world.find_planet( "construct" ) == nullptr );

		world.create_planet( "construct", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
		BOOST_CHECK( world.get_num_planets() == 1 );

		const Planet* planet = world.find_planet( "construct" );
		BOOST_REQUIRE( planet != nullptr );

		BOOST_CHECK( planet->get_id() == "construct" );
		BOOST_CHECK( planet->get_size() == Planet::Vector( 4, 4, 4 ) );
		BOOST_CHECK( planet->get_chunk_size() == Chunk::Vector( 16, 16, 16 ) );

		// Another planet.
		world.create_planet( "void", Planet::Vector( 2, 2, 2 ), Chunk::Vector( 8, 8, 8 ) );
		BOOST_CHECK( world.get_num_planets() == 2 );

		planet = world.find_planet( "void" );
		BOOST_REQUIRE( planet != nullptr );

		BOOST_CHECK( planet->get_id() == "void" );
		BOOST_CHECK( planet->get_size() == Planet::Vector( 2, 2, 2 ) );
		BOOST_CHECK( planet->get_chunk_size() == Chunk::Vector( 8, 8, 8 ) );

		// Check construct again.
		planet = world.find_planet( "construct" );
		BOOST_REQUIRE( planet != nullptr );

		BOOST_CHECK( planet->get_id() == "construct" );
		BOOST_CHECK( planet->get_size() == Planet::Vector( 4, 4, 4 ) );
		BOOST_CHECK( planet->get_chunk_size() == Chunk::Vector( 16, 16, 16 ) );
	}

	// Iterate over planets.
	{
		World world;

		world.create_planet( "planet0", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
		world.create_planet( "planet1", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
		world.create_planet( "planet2", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );

		World::PlanetConstIterator iter = world.planets_begin();
		World::PlanetConstIterator iter_end = world.planets_end();

		bool planets[3] = {false, false, false};

		for( ; iter != iter_end; ++iter ) {
			if( iter->first == "planet0" ) {
				planets[0] = true;
			}
			else if( iter->first == "planet1" ) {
				planets[1] = true;
			}
			else if( iter->first == "planet2" ) {
				planets[2] = true;
			}
		}

		BOOST_CHECK( planets[0] == true );
		BOOST_CHECK( planets[1] == true );
		BOOST_CHECK( planets[2] == true );
	}

	// Manage classes.
	{
		FlexID id;
		id.parse( "id.base/grass" );

		Class cls( id );

		World world;
		BOOST_CHECK( world.find_class( id ) == nullptr );

		world.add_class( cls );
		BOOST_CHECK( world.find_class( id ) != nullptr );
		BOOST_CHECK( world.find_class( id ) != &cls );
	}

	// Create entities.
	{
		FlexID id = FlexID::make( "id.base/ball" );

		Class cls( id );

		World world;
		world.add_class( cls );

		{
			Entity& ent = world.create_entity( id );
			BOOST_CHECK( ent.get_id() == 0 );
			BOOST_CHECK( world.get_num_entities() == 1 );
			BOOST_CHECK( &ent.get_class() == world.find_class( id ) );
			BOOST_CHECK( &ent == world.find_entity( 0 ) );
		}

		{
			Entity& ent = world.create_entity( id );
			BOOST_CHECK( &ent.get_class() == world.find_class( id ) );
			BOOST_CHECK( world.get_num_entities() == 2 );
			BOOST_CHECK( ent.get_id() == 1 );
			BOOST_CHECK( &ent == world.find_entity( 1 ) );
		}

		BOOST_CHECK( world.find_entity( 0 ) != world.find_entity( 1 ) );
		BOOST_CHECK( world.find_entity( 2 ) == nullptr );
	}

	// Delete entities.
	{
		FlexID id = FlexID::make( "id.base/ball" );

		Class cls( id );

		World world;
		world.add_class( cls );

		Entity& ent = world.create_entity( id );
		BOOST_CHECK( world.get_num_entities() == 1 );
		BOOST_REQUIRE( ent.get_id() == 0 );

		world.delete_entity( 0 );
		BOOST_CHECK( world.get_num_entities() == 0 );
		BOOST_CHECK( world.find_entity( 0 ) == nullptr );
	}

	// Edit entity->planet relations.
	{
		World world;
		world.create_planet( "construct", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
		BOOST_REQUIRE( world.find_planet( "construct" ) != nullptr );

		Class cls( FlexID::make( "fw.base/ball" ) );
		world.add_class( cls );

		Entity& ent = world.create_entity( cls.get_id() );
		BOOST_REQUIRE( ent.get_id() == 0 );

		world.link_entity_to_planet( 0, "construct" );
		BOOST_CHECK( world.find_planet( "construct" )->has_entity( ent ) );
		BOOST_CHECK( world.find_linked_planet( 0 ) == world.find_planet( "construct" ) );

		// Re-link.
		world.link_entity_to_planet( 0, "construct" );
		BOOST_CHECK( world.find_planet( "construct" )->has_entity( ent ) );
		BOOST_CHECK( world.find_linked_planet( 0 ) == world.find_planet( "construct" ) );

		// Unlink.
		world.unlink_entity_from_planet( 0 );
		BOOST_CHECK( world.find_planet( "construct" )->has_entity( ent ) == false );
		BOOST_CHECK( world.find_linked_planet( 0 ) == nullptr );

		// Link again.
		world.link_entity_to_planet( 0, "construct" );
		BOOST_CHECK( world.find_planet( "construct" )->has_entity( ent ) );
		BOOST_CHECK( world.find_linked_planet( 0 ) == world.find_planet( "construct" ) );

		// Link to another planet.
		world.create_planet( "foobar", Planet::Vector( 2, 2, 2 ), Chunk::Vector( 16, 16, 16 ) );
		BOOST_REQUIRE( world.find_planet( "foobar" ) != nullptr );

		world.link_entity_to_planet( 0, "foobar" );
		BOOST_CHECK( world.find_planet( "construct" )->has_entity( ent ) == false );
		BOOST_CHECK( world.find_planet( "foobar" )->has_entity( ent ) == true );
		BOOST_CHECK( world.find_linked_planet( 0 ) == world.find_planet( "foobar" ) );
	}

	// Attach & detach entities.
	{
		static const FlexID CLASS_ID = FlexID::make( "some/class" );
		static const std::string PLANET_ID = "foo";

		// Attach after adding parent to world.
		{
			// Setup world.
			Class cls( CLASS_ID );
			cls.set_hook( "hook", sf::Vector3f( 0, 0, 0 ) );

			World world;

			world.add_class( cls );
			world.create_planet( PLANET_ID, Planet::Vector( 2, 2, 2 ), Chunk::Vector( 16, 16, 16 ) );

			// Setup entities.
			Entity& parent_entity = world.create_entity( CLASS_ID );
			Entity& child_entity = world.create_entity( CLASS_ID );

			child_entity.set_position( sf::Vector3f( 1, 2, 3 ) );

			// Link parent.
			world.link_entity_to_planet( parent_entity.get_id(), PLANET_ID );

			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 1 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );

			// Attach child.
			world.attach_entity( child_entity.get_id(), parent_entity.get_id(), "hook" );

			BOOST_CHECK( child_entity.get_parent() == &parent_entity );
			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 0, 0, 0 ) );
			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 1 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );
			BOOST_CHECK( parent_entity.get_num_children() == 1 );
		}

		// Attach to a non-existing hook.
		{
			// Setup world.
			Class cls( CLASS_ID );

			World world;

			world.add_class( cls );
			world.create_planet( PLANET_ID, Planet::Vector( 2, 2, 2 ), Chunk::Vector( 16, 16, 16 ) );

			// Make sure hook doesn't exist yet.
			BOOST_CHECK( world.find_class( CLASS_ID )->find_hook( "new_hook" ) == false );

			// Setup entities.
			Entity& parent_entity = world.create_entity( CLASS_ID );
			Entity& child_entity = world.create_entity( CLASS_ID );

			child_entity.set_position( sf::Vector3f( 1, 2, 3 ) );

			// Link parent.
			world.link_entity_to_planet( parent_entity.get_id(), PLANET_ID );

			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 1 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );

			// Attach child.
			world.attach_entity( child_entity.get_id(), parent_entity.get_id(), "new_hook" );

			BOOST_CHECK( child_entity.get_parent() == &parent_entity );
			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 0, 0, 0 ) );
			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 1 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );
			BOOST_CHECK( parent_entity.get_num_children() == 1 );

			// Check that new hook got created.
			const sf::Vector3f* hook = world.find_class( CLASS_ID )->find_hook( "new_hook" );

			BOOST_REQUIRE( hook != nullptr );
			BOOST_CHECK( *hook == Class::INVISIBLE_HOOK );
		}

		// Attach after adding both parent and child to world.
		{
			// Setup world.
			Class cls( CLASS_ID );
			cls.set_hook( "hook", sf::Vector3f( 0, 0, 0 ) );

			World world;

			world.add_class( cls );
			world.create_planet( PLANET_ID, Planet::Vector( 2, 2, 2 ), Chunk::Vector( 16, 16, 16 ) );

			// Setup entities.
			Entity& parent_entity = world.create_entity( CLASS_ID );
			Entity& child_entity = world.create_entity( CLASS_ID );

			child_entity.set_position( sf::Vector3f( 1, 2, 3 ) );

			// Link entities.
			world.link_entity_to_planet( parent_entity.get_id(), PLANET_ID );
			world.link_entity_to_planet( child_entity.get_id(), PLANET_ID );

			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 2 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() )->get_id() == PLANET_ID );

			// Attach child.
			world.attach_entity( child_entity.get_id(), parent_entity.get_id(), "hook" );

			BOOST_CHECK( child_entity.get_parent() == &parent_entity );
			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 0, 0, 0 ) );
			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 1 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );
			BOOST_CHECK( parent_entity.get_num_children() == 1 );
		}

		// Reattach to another entity.
		{
			// Setup world.
			Class cls( CLASS_ID );
			cls.set_hook( "hook", sf::Vector3f( 0, 0, 0 ) );

			World world;

			world.add_class( cls );
			world.create_planet( PLANET_ID, Planet::Vector( 2, 2, 2 ), Chunk::Vector( 16, 16, 16 ) );

			// Setup entities.
			Entity& parent_entity = world.create_entity( CLASS_ID );
			Entity& new_parent_entity = world.create_entity( CLASS_ID );
			Entity& child_entity = world.create_entity( CLASS_ID );

			child_entity.set_position( sf::Vector3f( 1, 2, 3 ) );

			// Link entities.
			world.link_entity_to_planet( parent_entity.get_id(), PLANET_ID );
			world.link_entity_to_planet( new_parent_entity.get_id(), PLANET_ID );
			world.link_entity_to_planet( child_entity.get_id(), PLANET_ID );

			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 3 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( new_parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() )->get_id() == PLANET_ID );

			// Attach child.
			world.attach_entity( child_entity.get_id(), parent_entity.get_id(), "hook" );

			BOOST_CHECK( child_entity.get_parent() == &parent_entity );
			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 0, 0, 0 ) );
			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 2 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( new_parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );
			BOOST_CHECK( parent_entity.get_num_children() == 1 );
			BOOST_CHECK( new_parent_entity.get_num_children() == 0 );

			// Reattach.
			world.attach_entity( child_entity.get_id(), new_parent_entity.get_id(), "hook" );

			BOOST_CHECK( child_entity.get_parent() == &new_parent_entity );
			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 0, 0, 0 ) );
			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 2 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( new_parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() ) == nullptr );
			BOOST_CHECK( parent_entity.get_num_children() == 0 );
			BOOST_CHECK( new_parent_entity.get_num_children() == 1 );
		}

		// Detach entity.
		{
			// Setup world.
			Class cls( CLASS_ID );
			cls.set_hook( "hook", sf::Vector3f( 0, 0, 0 ) );

			World world;

			world.add_class( cls );
			world.create_planet( PLANET_ID, Planet::Vector( 2, 2, 2 ), Chunk::Vector( 16, 16, 16 ) );

			// Setup entities.
			Entity& parent_entity = world.create_entity( CLASS_ID );
			Entity& child_entity = world.create_entity( CLASS_ID );

			parent_entity.set_position( sf::Vector3f( 1, 2, 3 ) );
			parent_entity.set_rotation( sf::Vector3f( 10, 20, 30 ) );
			child_entity.set_position( sf::Vector3f( 4, 5, 6 ) );

			// Link entities.
			world.link_entity_to_planet( parent_entity.get_id(), PLANET_ID );
			world.link_entity_to_planet( child_entity.get_id(), PLANET_ID );

			// Attach child.
			world.attach_entity( child_entity.get_id(), parent_entity.get_id(), "hook" );

			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 0, 0, 0 ) );

			// Detach.
			world.detach_entity( child_entity.get_id() );

			BOOST_CHECK( child_entity.get_parent() == nullptr );
			BOOST_CHECK( world.find_planet( PLANET_ID )->get_num_entities() == 2 );
			BOOST_CHECK( world.find_linked_planet( parent_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( world.find_linked_planet( child_entity.get_id() )->get_id() == PLANET_ID );
			BOOST_CHECK( parent_entity.get_num_children() == 0 );
			BOOST_CHECK( child_entity.get_position() == sf::Vector3f( 1, 2, 3 ) );
			BOOST_CHECK( child_entity.get_rotation() == sf::Vector3f( 10, 20, 30 ) );
		}
	}
}
