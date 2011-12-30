#include <FlexWorld/Class.hpp>
#include <FlexWorld/FlexID.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestClass ) {
	using namespace flex;

	// Create class and check initial state.
	{
		FlexID id;
		id.parse( "fw.weapons/sword.yml" );
		Class cls( id );

		BOOST_CHECK( cls.get_id() == id );
		BOOST_CHECK( &cls.get_id() != &id );

		BOOST_CHECK( cls.get_name() == "" );
		BOOST_CHECK( cls.get_origin() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( cls.get_num_textures() == 0 );
		BOOST_CHECK( cls.get_num_hooks() == 0 );
		BOOST_CHECK( cls.has_model() == false );
	}

	// Check basic properties.
	{
		FlexID id;
		id.parse( "fw.weapons/sword.yml" );
		Class cls( id );

		cls.set_name( "Sword" );
		BOOST_CHECK( cls.get_name() == "Sword" );

		cls.set_origin( sf::Vector3f( 1, 2, 3 ) );
		BOOST_CHECK( cls.get_origin() == sf::Vector3f( 1, 2, 3 ) );

		cls.set_model( Resource( id ) );
		BOOST_CHECK( cls.get_model().get_id() == id );
	}

	// Check adding textures.
	{
		FlexID class_id;
		class_id.parse( "fw.weapons/sword.yml" );
		Class cls( class_id );

		FlexID tex_id;
		tex_id.parse( "fw.weapons/sword.png" );
		Resource tex_resource( tex_id );

		cls.add_texture( tex_resource );
		BOOST_CHECK( cls.get_num_textures() == 1 );
		BOOST_CHECK( cls.has_texture( 0 ) == true );
		BOOST_CHECK( cls.get_texture( 0 ).get_id() == tex_id );
		BOOST_CHECK( &cls.get_texture( 0 ) != &tex_resource );

		cls.add_texture( tex_resource );
		BOOST_CHECK( cls.get_num_textures() == 2 );
		BOOST_CHECK( cls.has_texture( 1 ) == true );
		BOOST_CHECK( cls.get_texture( 1 ).get_id() == tex_id );
		BOOST_CHECK( &cls.get_texture( 1 ) != &tex_resource );
	}

	// Check setting hooks.
	{
		FlexID class_id;
		class_id.parse( "fw.weapons/sword.yml" );
		Class cls( class_id );

		sf::Vector3f vector( 1, 2, 3 );

		cls.set_hook( "default", vector );
		BOOST_CHECK( cls.get_num_hooks() == 1 );

		const sf::Vector3f* hook( cls.find_hook( "default" ) );
		BOOST_CHECK( *hook == sf::Vector3f( 1, 2, 3 ) );
		BOOST_CHECK( hook != &vector );
	}
}
