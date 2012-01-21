#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/LockFacility.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

bool g_sh_thread_running = false;

void run_host( flex::SessionHost* host ) {
	g_sh_thread_running = true;
	BOOST_REQUIRE( host->run() );
	g_sh_thread_running = false;
}

BOOST_AUTO_TEST_CASE( TestSessionHost ) {
	using namespace flex;

	// Initial state.
	{
		LockFacility lock_facility;
		AccountManager acc_mgr;
		SessionHost host( lock_facility, acc_mgr );

		BOOST_CHECK( &host.get_lock_facility() == &lock_facility );
		BOOST_CHECK( &host.get_account_manager() == &acc_mgr );
		BOOST_CHECK( host.get_ip() == "0.0.0.0" );
		BOOST_CHECK( host.get_port() == 2593 );
		BOOST_CHECK( host.get_auth_mode() == SessionHost::OPEN_AUTH );
	}

	// Basic properties.
	{
		LockFacility lock_facility;
		AccountManager acc_mgr;
		SessionHost host( lock_facility, acc_mgr );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_auth_mode( SessionHost::KEY_AUTH );

		BOOST_CHECK( host.get_ip() == "127.0.0.1" );
		BOOST_CHECK( host.get_port() == 2593 );
		BOOST_CHECK( host.get_auth_mode() == SessionHost::KEY_AUTH );
	}

	enum { TIMEOUT = 2000 };
	enum { WAIT_INTERVAL = 25 };

	// Run and stop.
	{
		LockFacility lock_facility;
		AccountManager acc_mgr;
		SessionHost host( lock_facility, acc_mgr );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );

		g_sh_thread_running = true;
		boost::thread host_thread( std::bind( &run_host, &host ) );

		{
			unsigned int time_passed = 0;
			while( time_passed < TIMEOUT && !host.is_running() && g_sh_thread_running ) {
				boost::this_thread::sleep( boost::posix_time::milliseconds( WAIT_INTERVAL ) );
				time_passed += WAIT_INTERVAL;
			}

			BOOST_REQUIRE( time_passed < TIMEOUT );
		}

		BOOST_REQUIRE( host.is_running() );

		// Stop and wait.
		host.stop();
		host_thread.join();
	}
}
