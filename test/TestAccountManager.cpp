#include "Config.hpp"

#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/Account.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestAccountManager ) {
	using namespace fw;

	// Initial state.
	{
		AccountManager mgr;

		BOOST_CHECK( mgr.get_num_accounts() == 0 );
	}

	// Add account.
	{
		Account acc;
		acc.set_username( "test" );
		acc.set_password( "foo" );

		AccountManager mgr;
		mgr.add_account( acc );

		BOOST_CHECK( mgr.get_num_accounts() == 1 );

		// Verify.
		const Account* acc_ptr( mgr.find_account( "test" ) );
		BOOST_CHECK( acc_ptr->get_username() == acc.get_username() );
		BOOST_CHECK( acc_ptr->get_password() == acc.get_password() );
	}

	// Add multiple accounts.
	{
		Account acc[3];

		acc[0].set_username( "0" );
		acc[0].set_password( "0" );
		acc[1].set_username( "1" );
		acc[1].set_password( "1" );
		acc[2].set_username( "2" );
		acc[2].set_password( "2" );

		AccountManager mgr;
		mgr.add_account( acc[0] );
		mgr.add_account( acc[1] );
		mgr.add_account( acc[2] );

		BOOST_CHECK( mgr.get_num_accounts() == 3 );

		const Account* ptr[3];
		ptr[0] = mgr.find_account( "0" );
		ptr[1] = mgr.find_account( "1" );
		ptr[2] = mgr.find_account( "2" );

		BOOST_CHECK( ptr[0] != nullptr );
		BOOST_CHECK( ptr[1] != nullptr );
		BOOST_CHECK( ptr[2] != nullptr );
		BOOST_CHECK( ptr[0] != ptr[1] && ptr[0] != ptr[2] && ptr[1] != ptr[2] );
	}

	// Remove account.
	{
		Account acc;
		acc.set_username( "Foo" );
		acc.set_password( "Bar" );

		AccountManager mgr;

		mgr.add_account( acc );
		BOOST_CHECK( mgr.get_num_accounts() == 1 );
		BOOST_CHECK( mgr.find_account( "Foo" ) != nullptr );

		mgr.remove_account( "Foo" );

		BOOST_CHECK( mgr.get_num_accounts() == 0 );
		BOOST_CHECK( mgr.find_account( "Foo" ) == nullptr );
	}

	// Update account.
	{
		Account acc;
		acc.set_username( "Foo" );
		acc.set_password( "Bar" );

		AccountManager mgr;
		mgr.add_account( acc );
		BOOST_CHECK( mgr.find_account( "Foo" )->get_password() == "Bar" );

		acc.set_password( "Meh" );
		mgr.update_account( acc );
		BOOST_CHECK( mgr.find_account( "Foo" )->get_password() == "Meh" );
	}

	// Load accounts from a directory.
	{
		AccountManager mgr;

		std::size_t num_loaded = mgr.load_accounts_from_directory( DATA_DIRECTORY + "/saves/test/accounts", AccountManager::CANCEL_ON_FAILURE );
		BOOST_CHECK( num_loaded == 0 );

		num_loaded = mgr.load_accounts_from_directory( DATA_DIRECTORY + "/saves/test/accounts", AccountManager::CONTINUE_ON_FAILURE );
		BOOST_CHECK( num_loaded == 2 );

		const Account* acc = mgr.find_account( "Tank" );
		BOOST_REQUIRE( acc != nullptr );
		BOOST_CHECK( acc->get_username() == "Tank" );
		BOOST_CHECK( acc->get_password() == "meowfoo" );

		acc = mgr.find_account( "John" );
		BOOST_REQUIRE( acc != nullptr );
		BOOST_CHECK( acc->get_username() == "John" );
		BOOST_CHECK( acc->get_password() == "meowfoo" );
	}
}
