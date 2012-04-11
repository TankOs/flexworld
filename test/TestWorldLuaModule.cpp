#include "Config.hpp"
#include "LuaUtils.hpp"
#include "ExampleWorldGate.hpp"

#include <FlexWorld/LuaModules/World.hpp>
#include <FlexWorld/LuaModules/Test.hpp>

#include <Diluculum/LuaState.hpp>
#include <boost/test/unit_test.hpp>

void setup_state_for_world( Diluculum::LuaState& state ) {
	state["flex"] = Diluculum::EmptyTable;

	flex::lua::World::register_class( state["flex"]["World"] );
	flex::lua::Test::register_class( state["flex"]["Test"] );
}

BOOST_AUTO_TEST_CASE( TestWorldLuaModule ) {
	using namespace flex;

	// Initial state.
	{
		Diluculum::LuaState state;
		setup_state_for_world( state );

		// Valid ctor.
		{
			ExampleWorldGate gate;
			lua::World world( gate );
		}

		// Lua ctor.
		{
			BOOST_CHECK( check_error( "Not allowed to instantiate World.", "foo = flex.World.new()", state ) == true );
		}
	}

	// World gate.
	{
		Diluculum::LuaState state;
		setup_state_for_world( state );

		ExampleWorldGate gate;
		lua::World world( gate );
		world.register_object( state["flex"]["world"] );

		BOOST_CHECK_NO_THROW( state.doString( "flex.world:destroy_block( {1, 2, 3}, \"foobar\" )" ) );
	}

	// Call functions with invalid arguments.
	{
		Diluculum::LuaState state;
		setup_state_for_world( state );

		ExampleWorldGate gate;
		lua::World world( gate );
		world.register_object( state["flex"]["world"] );

		// destroy_block
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:destroy_block()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:destroy_block( {1, 2, 3} )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:destroy_block( {1, 2, 3}, \"foobar\", 555 )", state ) == true );

		BOOST_CHECK( check_error( "Expected table for position.", "flex.world:destroy_block( 123, \"foobar\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of elements in position table.", "flex.world:destroy_block( {1, 2}, \"foobar\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of elements in position table.", "flex.world:destroy_block( {1, 2, 3, 4}, \"foobar\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for x position.", "flex.world:destroy_block( {\"a\", 2, 3}, \"foobar\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for y position.", "flex.world:destroy_block( {1, \"a\", 3}, \"foobar\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for z position.", "flex.world:destroy_block( {1, 2, \"a\"}, \"foobar\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for planet.", "flex.world:destroy_block( {1, 2, 3}, 0 )", state ) == true );
		BOOST_CHECK( check_error( "Invalid planet.", "flex.world:destroy_block( {1, 2, 3}, \"\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid planet.", "flex.world:destroy_block( {1, 2, 3}, \"___\" )", state ) == true );
	}
}
