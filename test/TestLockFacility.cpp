#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

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

	// Lock planets.
	{
		Planet foo( "foo", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
		Planet bar( "bar", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
		Planet baz( "baz", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );

		LockFacility facility;

		facility.lock_planet( foo, true );
		BOOST_CHECK( facility.is_planet_locked( foo ) == true );

		facility.lock_planet( foo, false );
		BOOST_CHECK( facility.is_planet_locked( foo ) == false );

		// Lock twice.
		facility.lock_planet( foo, true );
		BOOST_CHECK( facility.is_planet_locked( foo ) == true );
		facility.lock_planet( foo, true );
		BOOST_CHECK( facility.is_planet_locked( foo ) == true );

		facility.lock_planet( foo, false );
		BOOST_CHECK( facility.is_planet_locked( foo ) == true );
		facility.lock_planet( foo, false );
		BOOST_CHECK( facility.is_planet_locked( foo ) == false );

		facility.lock_planet( foo, true );
		facility.lock_planet( bar, true );
		facility.lock_planet( baz, true );
		BOOST_CHECK( facility.is_planet_locked( foo ) == true );
		BOOST_CHECK( facility.is_planet_locked( bar ) == true );
		BOOST_CHECK( facility.is_planet_locked( baz ) == true );

		facility.lock_planet( foo, false );
		facility.lock_planet( bar, false );
		facility.lock_planet( baz, false );
		BOOST_CHECK( facility.is_planet_locked( foo ) == false );
		BOOST_CHECK( facility.is_planet_locked( bar ) == false );
		BOOST_CHECK( facility.is_planet_locked( baz ) == false );
	}

	// Lock entities.
	{
		Class cls( FlexID::make( "foo/bar" ) );

		Entity foo( cls );
		Entity bar( cls );
		Entity baz( cls );

		LockFacility facility;

		facility.lock_entity( foo, true );
		BOOST_CHECK( facility.is_entity_locked( foo ) == true );

		facility.lock_entity( foo, false );
		BOOST_CHECK( facility.is_entity_locked( foo ) == false );

		// Lock twice.
		facility.lock_entity( foo, true );
		BOOST_CHECK( facility.is_entity_locked( foo ) == true );
		facility.lock_entity( foo, true );
		BOOST_CHECK( facility.is_entity_locked( foo ) == true );

		facility.lock_entity( foo, false );
		BOOST_CHECK( facility.is_entity_locked( foo ) == true );
		facility.lock_entity( foo, false );
		BOOST_CHECK( facility.is_entity_locked( foo ) == false );

		facility.lock_entity( foo, true );
		facility.lock_entity( bar, true );
		facility.lock_entity( baz, true );
		BOOST_CHECK( facility.is_entity_locked( foo ) == true );
		BOOST_CHECK( facility.is_entity_locked( bar ) == true );
		BOOST_CHECK( facility.is_entity_locked( baz ) == true );

		facility.lock_entity( foo, false );
		facility.lock_entity( bar, false );
		facility.lock_entity( baz, false );
		BOOST_CHECK( facility.is_entity_locked( foo ) == false );
		BOOST_CHECK( facility.is_entity_locked( bar ) == false );
		BOOST_CHECK( facility.is_entity_locked( baz ) == false );
	}
}
