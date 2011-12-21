//#include <FlexWorld/Class.hpp>
//#include <FlexWorld/ClassCache.hpp>
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
