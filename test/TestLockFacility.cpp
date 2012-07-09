#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

#include <boost/test/unit_test.hpp>

bool g_main_thread_wait = false;
boost::mutex g_main_thread_wait_mutex;

void planet_thread_func( fw::LockFacility* facility ) {
}

BOOST_AUTO_TEST_CASE( TestLockFacility ) {
	using namespace fw;

	// Initial state.
	{
		LockFacility facility;

		BOOST_CHECK( facility.is_account_manager_locked() == false );
		BOOST_CHECK( facility.is_world_locked() == false );
		BOOST_CHECK( facility.get_num_planet_locks() == 0 );
		BOOST_CHECK( facility.get_num_locked_planets() == 0 );
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

	// Lock world.
	{
		LockFacility facility;

		facility.lock_world( true );
		BOOST_CHECK( facility.is_world_locked() == true );

		facility.lock_world( false );
		BOOST_CHECK( facility.is_world_locked() == false );

		// Lock twice.
		facility.lock_world( true );
		BOOST_CHECK( facility.is_world_locked() == true );
		facility.lock_world( true );
		BOOST_CHECK( facility.is_world_locked() == true );

		facility.lock_world( false );
		BOOST_CHECK( facility.is_world_locked() == true );
		facility.lock_world( false );
		BOOST_CHECK( facility.is_world_locked() == false );
	}

	// Planets.
	{
		LockFacility facility;
		Planet foo( "foo", Planet::Vector( 1, 1, 1 ), Chunk::Vector( 1, 1, 1 ) );

		facility.create_planet_lock( foo );
		BOOST_CHECK( facility.get_num_planet_locks() == 1 );
		BOOST_CHECK( facility.get_num_locked_planets() == 0 );
		BOOST_CHECK( facility.is_planet_locked( foo ) == false );

		// Lock.
		facility.lock_planet( foo, true );
		BOOST_CHECK( facility.get_num_locked_planets() == 1 );
		BOOST_CHECK( facility.is_planet_locked( foo ) == true );

		// Unlock.
		facility.lock_planet( foo, false );
		BOOST_CHECK( facility.get_num_locked_planets() == 0 );
		BOOST_CHECK( facility.is_planet_locked( foo ) == false );

		// Cleanup.
		facility.destroy_planet_lock( foo );
		BOOST_CHECK( facility.get_num_planet_locks() == 0 );
		BOOST_CHECK( facility.get_num_locked_planets() == 0 );
	}
}
