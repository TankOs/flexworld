#include "Config.hpp"
#include <FlexWorld/AccountDriver.hpp>
#include <FlexWorld/Account.hpp>

#include <fstream>
#include <sstream>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestAccountDriver ) {
	using namespace flex;

	// Load test data.
	std::ifstream in( DATA_DIRECTORY + "/accounts/Tank.yml" );
	BOOST_REQUIRE( in.is_open() );

	std::stringstream buffer;
	buffer << in.rdbuf();

	// Deserialize account.
	{
		Account acc;
		BOOST_CHECK_NO_THROW( acc = AccountDriver::deserialize( buffer.str() ) );

		// Verify data.
		BOOST_CHECK( acc.get_username() == "Tank" );
		BOOST_CHECK( acc.get_password() == "meowfoo" );
		BOOST_CHECK( acc.get_entity_id() == 1337 );
	}

	// Serialize.
	{
		Account acc;
		acc.set_username( "Tank" );
		acc.set_password( "meowfoo" );
		acc.set_entity_id( 1337 );

		std::string output = AccountDriver::serialize( acc );
		BOOST_CHECK( output == buffer.str() );
	}
}
