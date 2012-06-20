#include "Config.hpp"
#include "ExceptionChecker.hpp"

#include <FlexWorld/ClassDriver.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestClassDriver ) {
	using namespace flex;

	// Load class.
	{
		BOOST_CHECK_NO_THROW( ClassDriver::load( DATA_DIRECTORY + "/packages/test/grass.yml" ) );
		Class cls = ClassDriver::load( DATA_DIRECTORY + "/packages/test/grass.yml" );

		// Verify loaded data.
		BOOST_CHECK( cls.get_id().get() == "test/grass" );
		BOOST_CHECK( cls.get_name() == "Grass" );
		BOOST_CHECK( cls.get_origin() == sf::Vector3f( 0.1f, 0.2f, 0.3f ) );
		BOOST_CHECK( cls.get_scale() == sf::Vector3f( 0.11f, 0.22f, 0.33f ) );

		BOOST_CHECK( cls.get_num_hooks() == 2 );

		const sf::Vector3f* hook( nullptr );
		hook = cls.find_hook( "top" );
		BOOST_CHECK( hook && *hook == sf::Vector3f( 0.1f, 0.2f, 0.3f ) );
		hook = cls.find_hook( "bottom" );
		BOOST_CHECK( hook && *hook == sf::Vector3f( 1.0f, 0.9f, 0.8f ) );

		BOOST_CHECK( cls.get_num_textures() == 1 );

		BOOST_REQUIRE( cls.has_texture( 0 ) == true );
		BOOST_CHECK( cls.get_texture( 0 ).get_id().get() == "test/grass.png" );

		BOOST_CHECK( cls.has_model() && cls.get_model().get_id().get() == "test/cube.fwm" );

		BOOST_CHECK( cls.get_bounding_box() == FloatCuboid( 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f ) );

		BOOST_REQUIRE( cls.has_container_image() == true );
		BOOST_CHECK( cls.get_container_image().get_id().get() == "test/image.png" );
	}

	// Load class with reserved hook names.
	{
		BOOST_CHECK_EXCEPTION(
			ClassDriver::load( DATA_DIRECTORY + "/packages/test/reserved_hook.yml" ),
			ClassDriver::LoadException,
			ExceptionChecker<ClassDriver::LoadException>( "Invalid use of reserved hook name." )
		);
	}
}
