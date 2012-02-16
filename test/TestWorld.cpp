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
}
