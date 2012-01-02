#include <boost/test/unit_test.hpp>

#include <FlexWorld/Account.hpp>

BOOST_AUTO_TEST_CASE( TestAccount ) {
	using namespace flex;

	// Initial state.
	{
		Account acc;

		BOOST_CHECK( acc.get_username() == "" );
		BOOST_CHECK( acc.get_password() == "" );
	}

	// Set properties.
	{
		Account acc;

		acc.set_username( "John" );
		acc.set_password( "Doe" );

		BOOST_CHECK( acc.get_username() == "John" );
		BOOST_CHECK( acc.get_password() == "Doe" );
	}
}
