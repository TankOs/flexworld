#include <FlexWorld/SaveInfo.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSaveInfo ) {
	using namespace fw;

	// Initial state.
	{
		SaveInfo info;

		BOOST_CHECK( info.get_name() == "" );
		BOOST_CHECK( info.get_timestamp() == 0 );
		BOOST_CHECK( info.get_entities_path() == "" );
		BOOST_CHECK( info.get_planets_path() == "" );
		BOOST_CHECK( info.get_accounts_path() == "" );
	}

	// Set properties and verify.
	{
		SaveInfo info;

		info.set_name( "Name" );
		info.set_timestamp( 12345 );
		info.set_entities_path( "entities.fwe" );
		info.set_planets_path( "planets" );
		info.set_accounts_path( "accounts" );

		BOOST_CHECK( info.get_name() == "Name" );
		BOOST_CHECK( info.get_timestamp() == 12345 );
		BOOST_CHECK( info.get_entities_path() == "entities.fwe" );
		BOOST_CHECK( info.get_planets_path() == "planets" );
		BOOST_CHECK( info.get_accounts_path() == "accounts" );
	}
}
