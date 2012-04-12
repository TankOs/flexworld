#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/FlexID.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestEntity ) {
	using namespace flex;

	FlexID id;
	id.parse( "fw.weapons/sword" );

	Class sword_cls( id );

	// Initial state.
	{
		Entity ent( sword_cls );

		BOOST_CHECK( ent.get_id() == 0 );
		BOOST_CHECK( &ent.get_class() == &sword_cls );
		BOOST_CHECK( ent.get_name() == sword_cls.get_name() );
		BOOST_CHECK( ent.get_amount() == 1 );
		BOOST_CHECK( ent.get_position() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( ent.get_rotation() == sf::Vector3f( 0, 0, 0 ) );
	}

	// Basic properties.
	{
		Entity ent( sword_cls );

		ent.set_id( 123 );
		ent.set_amount( 333 );
		ent.set_position( sf::Vector3f( 4, 5, 6 ) );
		ent.set_rotation( sf::Vector3f( 7, 8, 9 ) );

		BOOST_CHECK( ent.get_id() == 123 );
		BOOST_CHECK( ent.get_amount() == 333 );
		BOOST_CHECK( ent.get_position() == sf::Vector3f( 4, 5, 6 ) );
		BOOST_CHECK( ent.get_rotation() == sf::Vector3f( 7, 8, 9 ) );
	}

	// Custom/class name.
	{
		Entity ent( sword_cls );
		std::string old_name( ent.get_name() );
		BOOST_CHECK( ent.get_name() != "CustomName" );

		ent.set_name( "CustomName" );
		BOOST_CHECK( ent.get_name() == "CustomName" );

		ent.reset_name();
		BOOST_CHECK( ent.get_name() == old_name );
	}

	// Copy ctor.
	{
		Entity ent0( sword_cls );

		ent0.set_id( 123 );
		ent0.set_amount( 333 );
		ent0.set_position( sf::Vector3f( 4, 5, 6 ) );
		ent0.set_rotation( sf::Vector3f( 7, 8, 9 ) );
		ent0.set_name( "CustomName" );

		Entity ent1( ent0 );
		BOOST_CHECK( ent0.get_id() == ent1.get_id() );
		BOOST_CHECK( &ent0.get_class() == &ent1.get_class() );
		BOOST_CHECK( ent0.get_name() == ent1.get_name() );
		BOOST_CHECK( ent0.get_amount() == ent1.get_amount() );
		BOOST_CHECK( ent0.get_position() == ent1.get_position() );
		BOOST_CHECK( ent0.get_rotation() == ent1.get_rotation() );
	}

	// Assignment operator.
	{
		Entity ent0( sword_cls );

		ent0.set_id( 123 );
		ent0.set_amount( 333 );
		ent0.set_position( sf::Vector3f( 4, 5, 6 ) );
		ent0.set_rotation( sf::Vector3f( 7, 8, 9 ) );
		ent0.set_name( "CustomName" );

		Class fb_cls( FlexID::make( "foo/bar" ) );
		Entity ent1( fb_cls );
		ent1 = ent0;

		BOOST_CHECK( ent0.get_id() == ent1.get_id() );
		BOOST_CHECK( &ent0.get_class() == &ent1.get_class() );
		BOOST_CHECK( ent0.get_name() == ent1.get_name() );
		BOOST_CHECK( ent0.get_amount() == ent1.get_amount() );
		BOOST_CHECK( ent0.get_position() == ent1.get_position() );
		BOOST_CHECK( ent0.get_rotation() == ent1.get_rotation() );
	}
}
