#include <FlexWorld/Class.hpp>
#include <FlexWorld/ClassCache.hpp>
#include <FlexWorld/FlexID.hpp>
#include <FlexWorld/Resource.hpp>
//#include <FlexWorld/Chunk.hpp>
//#include <FlexWorld/Planet.hpp>
//#include <FlexWorld/SHA1.hpp>
//#include <FlexWorld/Account.hpp>
//#include <FlexWorld/AccountDriver.hpp>
//#include <FlexWorld/MessageMeta.hpp>
//#include <FlexWorld/Message.hpp>

#include <iostream>

#define BOOST_TEST_MODULE FlexWorld
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestFlexID ) {
	using namespace flex;

	// Create FlexID and check initial state.
	{
		FlexID id;

		BOOST_CHECK( id.get() == "" );
		BOOST_CHECK( id.get_package() == "" );
		BOOST_CHECK( id.get_resource() == "" );
		BOOST_CHECK( id.is_valid_package() == false );
		BOOST_CHECK( id.is_valid_resource() == false );
	}

	// Check package character range.
	{
		const std::string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		for( short code = 0; code < 256; ++code ) {
			char ch = static_cast<char>( code );
			bool should_be_valid = valid.find( ch ) != std::string::npos;
			std::string package;

			package += ch;

			FlexID id;
			BOOST_CHECK( id.set_package( package ) == should_be_valid );
		}

		// Some special cases.
		FlexID id;
		BOOST_CHECK( id.set_package( ".foo" ) == false );
		BOOST_CHECK( id.set_package( "foo." ) == false );
		BOOST_CHECK( id.set_package( "" ) == false );
	}

	// Check setting package.
	{
		FlexID id;

		BOOST_CHECK( id.set_package( "fw.weapons" ) );
		BOOST_CHECK( id.get_package() == "fw.weapons" );
	}

	// Check setting invalid package recovers previous package.
	{
		FlexID id;

		id.set_package( "fw.weapons" );
		id.set_package( "" );

		BOOST_CHECK( id.get_package() == "fw.weapons" );
	}

	// Check setting resource without package.
	{
		FlexID id;
		BOOST_CHECK( id.set_resource( "sword.png" ) == false );
	}

	// Check resource character range.
	{
		const std::string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.";
		FlexID id;

		// Set package, won't work without.
		id.set_package( "fw.weapons" );

		for( short code = 0; code < 256; ++code ) {
			char ch( static_cast<char>( code ) );
			std::string resource;
			bool should_be_valid = valid.find( ch ) != std::string::npos;

			resource += ch;

			BOOST_CHECK( id.set_resource( resource ) == should_be_valid );
		}

		// Special case.
		BOOST_CHECK( id.set_resource( "" ) == false );
	}

	// Check setting resource.
	{
		FlexID id;
		id.set_package( "fw.weapons" );

		BOOST_CHECK( id.set_resource( "sword.png" ) == true );
		BOOST_CHECK( id.get_resource() == "sword.png" );
	}

	// Check setting invalid resource will recover previous one.
	{
		FlexID id;
		id.set_package( "fw.weapons" );
		id.set_resource( "sword.png" );
		id.set_resource( "" );

		BOOST_CHECK( id.get_resource() == "sword.png" );
	}

	// Check changing package won't alter resource.
	{
		FlexID id;

		id.set_package( "fw.weapons" );
		id.set_resource( "sword.png" );
		id.set_package( "fw.another" );

		BOOST_CHECK( id.get_resource() == "sword.png" );
	}

	// Check for full IDs.
	{
		// Both with package and resource.
		{
			FlexID id;
			id.set_package( "fw.package" );
			id.set_resource( "res.yml" );
			BOOST_CHECK( id.get() == "fw.package/res.yml" );
		}

		// With package only.
		{
			FlexID id;
			id.set_package( "fw.package" );
			BOOST_CHECK( id.get() == "fw.package" );
		}

		// Without anything.
		{
			FlexID id;
			BOOST_CHECK( id.get() == "" );
		}
	}

	// Check parsing full IDs.
	{
		FlexID id;

		BOOST_CHECK( id.parse( "fw.weapons/sword.png" ) == true );
		BOOST_CHECK( id.get_package() == "fw.weapons" );
		BOOST_CHECK( id.get_resource() == "sword.png" );

		BOOST_CHECK( id.parse( "fw/sword.png" ) == true );
		BOOST_CHECK( id.get_package() == "fw" );
		BOOST_CHECK( id.get_resource() == "sword.png" );

		BOOST_CHECK( id.parse( "fw.monsters.evil/sword.png" ) == true );
		BOOST_CHECK( id.get_package() == "fw.monsters.evil" );
		BOOST_CHECK( id.get_resource() == "sword.png" );

		BOOST_CHECK( id.parse( "fw.monsters.evil" ) == true );
		BOOST_CHECK( id.get_package() == "fw.monsters.evil" );
		BOOST_CHECK( id.get_resource() == "" );

		BOOST_CHECK( id.parse( "" ) == false );
		BOOST_CHECK( id.parse( "/" ) == false );
		BOOST_CHECK( id.parse( "foo.bar/" ) == false );
		BOOST_CHECK( id.parse( "/foo.bar" ) == false );
		BOOST_CHECK( id.parse( ".foo.bar" ) == false );
		BOOST_CHECK( id.parse( "foo.bar." ) == false );
		BOOST_CHECK( id.parse( "foo.bar./meh.foo" ) == false );
		BOOST_CHECK( id.parse( ".foo.bar/meh.foo" ) == false );
	}

	// Check parsing faulty full ID recovers previous state.
	{
		FlexID id;

		id.parse( "fw.weapons/sword.png" );
		id.parse( "" );

		BOOST_CHECK( id.get() == "fw.weapons/sword.png" );
	}

	// Check == and !=.
	{
		FlexID id0;
		FlexID id1;

		BOOST_CHECK( id0.parse( "fw/0" ) );
		BOOST_CHECK( id1.parse( "fw/0" ) );

		BOOST_CHECK( id0 == id1 );

		BOOST_CHECK( id1.parse( "fw/1" ) );

		BOOST_CHECK( id1 == id1 );
	}

	// Check for validity.
	{
		FlexID id;

		BOOST_CHECK( !id.is_valid_package() );
		BOOST_CHECK( !id.is_valid_resource() );

		id.set_package( "fw" );
		BOOST_CHECK( id.is_valid_package() );
		BOOST_CHECK( !id.is_valid_resource() );

		id.set_resource( "res" );
		BOOST_CHECK( id.is_valid_package() );
		BOOST_CHECK( id.is_valid_resource() );
	}
}

BOOST_AUTO_TEST_CASE( TestResource ) {
	using namespace flex;

	// Create resource and check initial state.
	{
		FlexID id;
		id.parse( "fw.weapons/sword.png" );
		Resource resource( id );

		BOOST_CHECK( resource.get_id() == id );
		BOOST_CHECK( &resource.get_id() != &id );
	}
}

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

BOOST_AUTO_TEST_CASE( TestClassCache ) {
	using namespace flex;

	// Create cache and check initial state.
	{
		ClassCache cache;

		BOOST_CHECK( cache.get_num_cached_classes() == 0 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	FlexID class_id;
	class_id.parse( "fw.weapons/sword.yml" );

	Class cls( class_id );
	Class cls2( class_id );

	// Cache new class.
	{
		ClassCache cache;
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( &cache.get_class( 0 ) == &cls );
		BOOST_CHECK( cache.get_num_cached_classes() == 1 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Cache same class twice.
	{
		ClassCache cache;
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( &cache.get_class( 0 ) == &cls );
		BOOST_CHECK( cache.get_num_cached_classes() == 1 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Cache two classes.
	{
		ClassCache cache;
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( cache.cache( cls2 ) == 1 );
		BOOST_CHECK( &cache.get_class( 0 ) == &cls );
		BOOST_CHECK( &cache.get_class( 1 ) == &cls2 );
		BOOST_CHECK( cache.get_num_cached_classes() == 2 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Forget without hole.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.forget( cls );

		BOOST_CHECK( cache.get_num_cached_classes() == 0 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Forget with hole.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.cache( cls2 );
		cache.forget( cls );

		BOOST_CHECK( cache.get_num_cached_classes() == 1 );
		BOOST_CHECK( cache.get_num_holes() == 1 );
		BOOST_CHECK( cache.is_id_valid( 0 ) == false );
	}

	// Fill hole.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.cache( cls2 );
		cache.forget( cls );

		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( cache.get_num_cached_classes() == 2 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}
}
