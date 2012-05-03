#include <FlexWorld/Class.hpp>
#include <FlexWorld/FlexID.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestClass ) {
	using namespace flex;

	// Create class and check initial state.
	{
		FlexID id;
		id.parse( "fw.weapons/sword" );
		Class cls( id );

		BOOST_CHECK( cls.get_id() == id );
		BOOST_CHECK( &cls.get_id() != &id );

		BOOST_CHECK( cls.get_name() == "" );
		BOOST_CHECK( cls.get_origin() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( cls.get_scale() == sf::Vector3f( 1, 1, 1 ) );
		BOOST_CHECK( cls.get_num_textures() == 0 );
		BOOST_CHECK( cls.get_num_hooks() == 0 );
		BOOST_CHECK( cls.has_model() == false );
		BOOST_CHECK( cls.has_bounding_box() == false );
	}

	// Check basic properties.
	{
		FlexID id;
		id.parse( "fw.weapons/sword" );
		Class cls( id );

		cls.set_name( "Sword" );
		cls.set_origin( sf::Vector3f( 1, 2, 3 ) );
		cls.set_model( Resource( id ) );
		cls.set_scale( sf::Vector3f( 5, 6, 7 ) );

		BOOST_CHECK( cls.get_name() == "Sword" );
		BOOST_CHECK( cls.get_origin() == sf::Vector3f( 1, 2, 3 ) );
		BOOST_CHECK( cls.get_model().get_id() == id );
		BOOST_CHECK( cls.get_scale() == sf::Vector3f( 5, 6, 7 ) );
	}

	// Bounding box.
	{
		FlexID id;
		id.parse( "fw.weapons/sword" );
		Class cls( id );

		BOOST_CHECK( cls.has_bounding_box() == false );

		cls.set_bounding_box( FloatCuboid( 1, 2, 3, 4, 5, 6 ) );

		BOOST_CHECK( cls.has_bounding_box() == true );
		BOOST_CHECK( cls.get_bounding_box() == FloatCuboid( 1, 2, 3, 4, 5, 6 ) );

		cls.set_bounding_box( FloatCuboid( 6, 5, 4, 3, 2, 1 ) );

		BOOST_CHECK( cls.has_bounding_box() == true );
		BOOST_CHECK( cls.get_bounding_box() == FloatCuboid( 6, 5, 4, 3, 2, 1 ) );

		cls.disable_bounding_box();
		BOOST_CHECK( cls.has_bounding_box() == false );
	}

	// Check adding textures.
	{
		FlexID class_id;
		class_id.parse( "fw.weapons/sword" );
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
		class_id.parse( "fw.weapons/sword" );
		Class cls( class_id );

		sf::Vector3f vector( 1, 2, 3 );

		cls.set_hook( "default", vector );
		BOOST_CHECK( cls.get_num_hooks() == 1 );

		const sf::Vector3f* hook( cls.find_hook( "default" ) );
		BOOST_CHECK( *hook == sf::Vector3f( 1, 2, 3 ) );
		BOOST_CHECK( hook != &vector );
	}

	// Assignment and copy ctor.
	{
		Class cls( FlexID::make( "fw.weapons/sword" ) );

		cls.set_name( "Sword" );
		cls.set_origin( sf::Vector3f( 1, 2, 3 ) );
		cls.set_model( Resource( FlexID::make( "a/model" ) ) );
		cls.set_scale( sf::Vector3f( 5, 6, 7 ) );

		cls.add_texture( FlexID::make( "texture/0" ) );
		cls.add_texture( FlexID::make( "texture/1" ) );
		cls.set_hook( "default", sf::Vector3f( 1, 2, 3 ) );
		cls.set_hook( "second", sf::Vector3f( 10, 20, 30 ) );

		struct Checker {
			static void check( const Class& first, const Class& second ) {
				BOOST_CHECK( second.get_id() == first.get_id() );
				BOOST_CHECK( second.get_name() == first.get_name() );
				BOOST_CHECK( second.get_origin() == first.get_origin() );
				BOOST_CHECK( second.get_model().get_id() == first.get_model().get_id() );
				BOOST_CHECK( second.get_scale() == first.get_scale() );

				BOOST_CHECK( second.get_num_textures() == first.get_num_textures() );
				BOOST_CHECK( second.get_texture( 0 ).get_id() == first.get_texture( 0 ).get_id() );
				BOOST_CHECK( second.get_texture( 1 ).get_id() == first.get_texture( 1 ).get_id() );

				BOOST_CHECK( second.get_num_hooks() == first.get_num_hooks() );
				BOOST_CHECK( *second.find_hook( "default" ) == *first.find_hook( "default" ) );
				BOOST_CHECK( *second.find_hook( "second" ) == *first.find_hook( "second" ) );
			}
		};

		// Assignment.
		{
			Class other( FlexID::make( "other/class" ) );
			other = cls;
			Checker::check( other, cls );
		}

		// Copy.
		{
			Class other( cls );
			Checker::check( other, cls );
		}
	}
}
