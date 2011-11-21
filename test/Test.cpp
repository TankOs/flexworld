#include <FlexWorld/Class.hpp>
#include <FlexWorld/ResourceId.hpp>

#include <iostream>

#define BOOST_TEST_MODULE FlexWorld
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( ResourceId ) {
	BOOST_CHECK_THROW( flex::ResourceId( ".invalid/thing" ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "invalid" ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "invalid/" ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "  " ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "/bleh" ), flex::ResourceId::ParserException );

	flex::ResourceId resource( "fw.weapons/sword" );

	BOOST_CHECK( resource.get_package_id() == "fw.weapons" );
	BOOST_CHECK( resource.get_path() == "sword" );
	BOOST_CHECK( resource.get() == "fw.weapons/sword" );

	flex::ResourceId same( "fw.weapons/sword" );
	BOOST_CHECK( resource == same );

	flex::ResourceId other( "fw.weapons/ball" );
	BOOST_CHECK( resource != other );
}

BOOST_AUTO_TEST_CASE( Class ) {
	flex::Class cls( flex::ResourceId( "fw.base/grass" ) );
	cls.set_name( "Grass" );
	cls.set_origin( sf::Vector3f( 1.f, 2.f, 3.f ) );

	flex::Resource tex0( flex::ResourceId( "fw.base/0.png" ) );
	flex::Resource tex1( flex::ResourceId( "fw.base/1.png" ) );
	flex::Resource tex2( flex::ResourceId( "fw.base/2.png" ) );
	cls.add_texture( tex0 );
	cls.add_texture( tex1 );
	cls.add_texture( tex2 );

	cls.set_hook( "default", sf::Vector3f( 123.f, 123.f, 123.f ) );
	cls.set_hook( "hand", sf::Vector3f( 555.f, 555.f, 555.f ) );

	BOOST_CHECK( cls.get_id().get() == "fw.base/grass" );
	BOOST_CHECK( cls.get_name() == "Grass" );

	BOOST_CHECK( cls.get_texture( 0 ) );
	BOOST_CHECK( cls.get_texture( 1 ) );
	BOOST_CHECK( cls.get_texture( 2 ) );
	BOOST_CHECK( !cls.get_texture( 3 ) );

	BOOST_CHECK( cls.get_texture( 0 )->get_id().get() == "fw.base/0.png" );
	BOOST_CHECK( cls.get_texture( 1 )->get_id().get() == "fw.base/1.png" );
	BOOST_CHECK( cls.get_texture( 2 )->get_id().get() == "fw.base/2.png" );

	BOOST_CHECK( cls.get_hook( "default" ) );
	BOOST_CHECK( cls.get_hook( "hand" ) );
	BOOST_CHECK( !cls.get_hook( "invalid" ) );

	BOOST_CHECK( *cls.get_hook( "default" ) == sf::Vector3f( 123.f, 123.f, 123.f ) );
	BOOST_CHECK( *cls.get_hook( "hand" ) == sf::Vector3f( 555.f, 555.f, 555.f ) );
}

