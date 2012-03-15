#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSessionHost ) {
	using namespace flex;

	// Initial state.
	{
		boost::asio::io_service service;
		LockFacility lock_facility;
		AccountManager acc_mgr;
		World world;
		SessionHost host( service, lock_facility, acc_mgr, world );

		BOOST_CHECK( &host.get_lock_facility() == &lock_facility );
		BOOST_CHECK( &host.get_account_manager() == &acc_mgr );
		BOOST_CHECK( &host.get_world() == &world );
		BOOST_CHECK( host.get_ip() == "0.0.0.0" );
		BOOST_CHECK( host.get_port() == 2593 );
		BOOST_CHECK( host.get_auth_mode() == SessionHost::OPEN_AUTH );
		BOOST_CHECK( host.get_player_limit() == 1 );
	}

	// Basic properties.
	{
		boost::asio::io_service service;
		LockFacility lock_facility;
		AccountManager acc_mgr;
		World world;
		SessionHost host( service, lock_facility, acc_mgr, world );

		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );
		host.set_auth_mode( SessionHost::KEY_AUTH );
		host.set_player_limit( 128 );

		BOOST_CHECK( host.get_ip() == "127.0.0.1" );
		BOOST_CHECK( host.get_port() == 2593 );
		BOOST_CHECK( host.get_auth_mode() == SessionHost::KEY_AUTH );
		BOOST_CHECK( host.get_player_limit() == 128 );
	}

	enum { TIMEOUT = 2000 };
	enum { WAIT_INTERVAL = 25 };

	// Run and stop.
	{
		boost::asio::io_service service;
		LockFacility lock_facility;
		AccountManager acc_mgr;
		World world;

		SessionHost host( service, lock_facility, acc_mgr, world );
		host.set_ip( "127.0.0.1" );
		host.set_port( 2593 );

		// Need to add fw.base.nature/grass and fw.base.nature/stone to world,
		// otherwise session host won't boot up.
		world.add_class( Class( FlexID::make( "fw.base.nature/grass" ) ) );
		world.add_class( Class( FlexID::make( "fw.base.nature/stone" ) ) ); // XXX 

		// Start host.
		BOOST_REQUIRE( host.start() );
	}
}
