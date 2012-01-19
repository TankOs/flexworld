#include <FlexWorld/LockFacility.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestLockFacility ) {
	using namespace flex;

	// Initial state.
	{
		LockFacility facility;

		BOOST_CHECK( facility.is_account_manager_locked() == false );
	}

	// Lock account manager.
	{
		LockFacility facility;

		facility.lock_account_manager( true );
		BOOST_CHECK( facility.is_account_manager_locked() == true );

		facility.lock_account_manager( false );
		BOOST_CHECK( facility.is_account_manager_locked() == false );

		// Lock 5 times.
		facility.lock_account_manager( true );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( true );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( true );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( true );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( true );
		BOOST_CHECK( facility.is_account_manager_locked() == true );

		facility.lock_account_manager( false );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( false );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( false );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( false );
		BOOST_CHECK( facility.is_account_manager_locked() == true );
		facility.lock_account_manager( false );
		BOOST_CHECK( facility.is_account_manager_locked() == false );
	}
}
