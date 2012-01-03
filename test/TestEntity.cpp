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
	}

	// Basic properties.
	{
		Entity ent( sword_cls );

		ent.set_id( 123 );
		ent.set_amount( 333 );
		ent.set_position( sf::Vector3f( 4, 5, 6 ) );

		BOOST_CHECK( ent.get_id() == 123 );
		BOOST_CHECK( ent.get_amount() == 333 );
		BOOST_CHECK( ent.get_position() == sf::Vector3f( 4, 5, 6 ) );
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
}
