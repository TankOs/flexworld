#include "Config.hpp"

#include <FlexWorld/SaveInfoDriver.hpp>
#include <FlexWorld/SaveInfo.hpp>

#include <boost/test/unit_test.hpp>
#include <fstream>

BOOST_AUTO_TEST_CASE( TestSaveInfoDriver ) {
	using namespace fw;

	// Load test data.
	std::ifstream in( DATA_DIRECTORY + "/saves/test/info.yml" );
	BOOST_REQUIRE( in.is_open() );

	std::stringstream buffer;
	buffer << in.rdbuf();

	// Deserialize.
	{
		SaveInfo info;

		BOOST_CHECK_NO_THROW( info = SaveInfoDriver::deserialize( buffer.str() ) );

		BOOST_CHECK( info.get_name() == "Unit test save" );
		BOOST_CHECK( info.get_timestamp() == 12345 );
		BOOST_CHECK( info.get_entities_path() == "entities.fwe" );
		BOOST_CHECK( info.get_planets_path() == "planets" );
		BOOST_CHECK( info.get_accounts_path() == "accounts" );
	}

	// Serialize.
	{
		SaveInfo info;
		info.set_name( "Unit test save" );
		info.set_timestamp( 12345 );
		info.set_entities_path( "entities.fwe" );
		info.set_planets_path( "planets" );
		info.set_accounts_path( "accounts" );

		std::string output = SaveInfoDriver::serialize( info );

		BOOST_CHECK( output == buffer.str() );
	}
}
