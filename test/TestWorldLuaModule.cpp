#include "Config.hpp"
#include "LuaUtils.hpp"
#include "ExampleWorldGate.hpp"
#include "ExceptionChecker.hpp"

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
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:destroy_block( {0, 0, 0}, \"foobar\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid block position." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:destroy_block( {1, 2, 3}, \"meow\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid planet." ) );

		BOOST_CHECK_NO_THROW( state.doString( "flex.world:set_block( {10, 20, 30}, \"planet\", \"some/class\" )" ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:set_block( {0, 0, 0}, \"planet\", \"some/class\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid block position." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:set_block( {10, 20, 30}, \"foobar\", \"some/class\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid planet." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:set_block( {10, 20, 30}, \"planet\", \"some/lass\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid class." ) );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.world:create_entity( \"some/class\", {11, 22, 33}, \"planet\" ) == 938 )" ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"foo/bar\", {11, 22, 33}, \"planet\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid class." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/class\", {0, 0, 0}, \"planet\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid entity position." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/class\", {11, 22, 33}, \"meow\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid planet." ) );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.world:create_entity( \"some/class348734\", 9384, \"foobar387\" ) == 458 )" ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/foo\", 9384, \"foobar387\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid class." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/class348734\", 2938, \"foobar387\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid parent entity ID." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/class348734\", 9384, \"foobar\" )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid hook." ) );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.world:create_entity( \"some/class12347882\", 3478834 ) == 784 )" ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/foo\", 3478834 )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid class." ) );
		BOOST_CHECK_EXCEPTION( state.doString( "flex.world:create_entity( \"some/class12347882\", 2938 )" ), std::runtime_error, ExceptionChecker<std::runtime_error>( "Invalid container ID." ) );

		BOOST_CHECK_NO_THROW( state.doString( "local pos, planet = flex.world:get_entity_position( 0 ); assert( pos[1] == 1 and pos[2] == 2 and pos[3] == 3 and planet == \"planet\" )" ) );
		BOOST_CHECK_EXCEPTION(
			state.doString( "flex.world:get_entity_position( 88 )" ),
			std::runtime_error,
			ExceptionChecker<std::runtime_error>( "Invalid entity ID." )
		);
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

		// set_block
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:set_block()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:set_block( {10, 20, 30} )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:set_block( {10, 20, 30}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:set_block( {10, 20, 30}, \"planet\", \"some/class\", 123 )", state ) == true );

		BOOST_CHECK( check_error( "Expected table for position.", "flex.world:set_block( 123, \"planet\", \"some/class\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of elements in position table.", "flex.world:set_block( {1, 2}, \"planet\", \"some/class\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of elements in position table.", "flex.world:set_block( {1, 2, 3, 4}, \"planet\", \"some/class\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for x position.", "flex.world:set_block( {\"a\", 2, 3}, \"planet\", \"some/class\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for y position.", "flex.world:set_block( {1, \"a\", 3}, \"planet\", \"some/class\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for z position.", "flex.world:set_block( {1, 2, \"a\"}, \"planet\", \"some/class\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for planet.", "flex.world:set_block( {1, 2, 3}, 0, \"some/class\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid planet.", "flex.world:set_block( {1, 2, 3}, \"\", \"some/class\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for class.", "flex.world:set_block( {1, 2, 3}, \"planet\", 123 )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class.", "flex.world:set_block( {1, 2, 3}, \"planet\", \"\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class.", "flex.world:set_block( {1, 2, 3}, \"planet\", \"package\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class.", "flex.world:set_block( {1, 2, 3}, \"planet\", \"faulty/class!\" )", state ) == true );

		// create_entity
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:create_entity()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:create_entity( \"some/class\", {1, 2, 3}, \"planet\", 123 )", state ) == true );

		BOOST_CHECK( check_error( "No matching function signature.", "flex.world:create_entity( \"some/class\", 123, 123 )", state ) == true );
		BOOST_CHECK( check_error( "No matching function signature.", "flex.world:create_entity( \"some/class\", {1, 2, 3}, 123 )", state ) == true );
		BOOST_CHECK( check_error( "No matching function signature.", "flex.world:create_entity( \"some/class\", \"foo\", 123 )", state ) == true );
		BOOST_CHECK( check_error( "No matching function signature.", "flex.world:create_entity( \"some/class\", \"foo\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for class.", "flex.world:create_entity( 123, {1, 2, 3}, \"planet\" )", state ) == true );

		BOOST_CHECK( check_error( "Invalid class.", "flex.world:create_entity( \"\", {1, 2, 3}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class.", "flex.world:create_entity( \"package\", {1, 2, 3}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class.", "flex.world:create_entity( \"faulty/class!\", {1, 2, 3}, \"planet\" )", state ) == true );

		BOOST_CHECK( check_error( "Wrong number of elements in position table.", "flex.world:create_entity( \"some/class\", {1, 2}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of elements in position table.", "flex.world:create_entity( \"some/class\", {1, 2, 3, 4}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for x position.", "flex.world:create_entity( \"some/class\", {\"a\", 2, 3}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for y position.", "flex.world:create_entity( \"some/class\", {1, \"a\", 3}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected number for z position.", "flex.world:create_entity( \"some/class\", {1, 2, \"a\"}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid x position.", "flex.world:create_entity( \"some/class\", {-1, 2, 3}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid y position.", "flex.world:create_entity( \"some/class\", {1, -1, 3}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid z position.", "flex.world:create_entity( \"some/class\", {1, 2, -1}, \"planet\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid planet.", "flex.world:create_entity( \"some/class\", {1, 2, 3}, \"\" )", state ) == true );

		BOOST_CHECK( check_error( "Invalid parent entity ID.", "flex.world:create_entity( \"some/class\", -1, \"foobar\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid hook.", "flex.world:create_entity( \"some/class\", 123, \"\" )", state ) == true );

		BOOST_CHECK( check_error( "Invalid parent entity ID.", "flex.world:create_entity( \"some/class\", -1 )", state ) == true );

		// get_entity_position
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:get_entity_position()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.world:get_entity_position( 1, 2 )", state ) == true );

		BOOST_CHECK( check_error( "Expected number for entity.", "flex.world:get_entity_position( \"123\" )", state ) == true );
	}
}
