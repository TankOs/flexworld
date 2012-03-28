#include "Config.hpp"

#include <FlexWorld/ClassLoader.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestClassLoader ) {
	using namespace flex;

	// Initial state.
	{
		ClassLoader loader;

		BOOST_CHECK( loader.get_num_search_paths() == 0 );
	}

	// Add search paths.
	{
		ClassLoader loader;

		loader.add_search_path( "some/path/0/" );
		loader.add_search_path( "some/path/1" ); // <-- Check slash is appended.
		loader.add_search_path( "some/path/2/" );

		BOOST_CHECK( loader.get_num_search_paths() == 3 );
		BOOST_CHECK( loader.get_search_path( 0 ) == "some/path/0/" );
		BOOST_CHECK( loader.get_search_path( 1 ) == "some/path/1/" );
		BOOST_CHECK( loader.get_search_path( 2 ) == "some/path/2/" );
	}

	// Load classes.
	{
		ClassLoader loader;

		loader.add_search_path( DATA_DIRECTORY + "/packages" );
		loader.add_search_path( DATA_DIRECTORY + "/more_packages" );

		Class cls( FlexID::make( "no/class" ) );

		BOOST_CHECK_NO_THROW( cls = loader.load( FlexID::make( "test/green" ) ) );
		BOOST_CHECK( cls.get_id().get() == "test/green" );

		BOOST_CHECK_NO_THROW( cls = loader.load( FlexID::make( "test/grass" ) ) );
		BOOST_CHECK( cls.get_id().get() == "test/grass" );

		BOOST_CHECK_NO_THROW( cls = loader.load( FlexID::make( "test/stone" ) ) );
		BOOST_CHECK( cls.get_id().get() == "test/stone" );
	}

	// Load non-existing class.
	{
		ClassLoader loader;
		loader.add_search_path( DATA_DIRECTORY + "/packages" );

		Class cls( FlexID::make( "no/class" ) );
		BOOST_CHECK_THROW( cls = loader.load( FlexID::make( "test/green" ) ), ClassLoader::LoadException );
	}

	// Load classes that reference invalid assets.
	{
		ClassLoader loader;
		loader.add_search_path( DATA_DIRECTORY + "/more_packages" );

		Class cls( FlexID::make( "no/class" ) );

		BOOST_CHECK_THROW( cls = loader.load( FlexID::make( "test/invalid_texture" ) ), ClassLoader::LoadException );
		BOOST_CHECK_THROW( cls = loader.load( FlexID::make( "test/invalid_model" ) ), ClassLoader::LoadException );
	}
}
