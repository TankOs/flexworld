#include <FlexWorld/World.hpp>
#include <FlexWorld/Planet.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestWorld ) {
	using namespace flex;

	// Initial state.
	{
		World world;

		BOOST_CHECK( world.get_num_planets() == 0 );
		BOOST_CHECK( world.get_num_entities() == 0 );
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
}
