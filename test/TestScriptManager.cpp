#include "Config.hpp"
#include "ExampleServerGate.hpp"
#include "ExampleWorldGate.hpp"

#include <FlexWorld/ScriptManager.hpp>
#include <FlexWorld/LuaModules/Event.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/Entity.hpp>

#include <SFML/System/String.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestScriptManager ) {
	using namespace fw;

	ExampleServerGate server_gate;
	ExampleWorldGate world_gate;

	// Initial state.
	{
		ScriptManager manager( server_gate, world_gate );

	}

	// Execute files.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_CHECK( manager.execute_file( DATA_DIRECTORY + "/scripts/some_funcs.lua" ) == true );
	}

	// Execute string.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_CHECK( manager.execute_string( "a = 5" ) == true );
	}

	// Verify last error is reset.
	{
		ScriptManager manager( server_gate, world_gate );

		// Strings.
		BOOST_CHECK( manager.execute_string( "this is invalid Lua code" ) == false );
		BOOST_CHECK( manager.get_last_error() != "" );

		BOOST_CHECK( manager.execute_string( "this_is_valid = 5" ) == true );
		BOOST_CHECK( manager.get_last_error() == "" );

		// Files.
		BOOST_CHECK( manager.execute_file( DATA_DIRECTORY + "/scripts/invalid.lua" ) == false );
		BOOST_CHECK( manager.get_last_error() != "" );

		BOOST_CHECK( manager.execute_file( DATA_DIRECTORY + "/scripts/some_funcs.lua" ) == true );
		BOOST_CHECK( manager.get_last_error() == "" );
	}

	// Clear Lua state.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_CHECK( manager.execute_string( "a = 5" ) == true );
		BOOST_CHECK( manager.execute_string( "assert( a == 5 )" ) == true );

		manager.clear();

		BOOST_CHECK( manager.execute_string( "assert( a == nil )" ) == true );
	}

	// Make sure modules are present.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_CHECK( manager.execute_string( "assert( fw.test ~= nil and fw.Test ~= nil )" ) == true );
		BOOST_CHECK( manager.execute_string( "assert( fw.event ~= nil and fw.Event ~= nil )" ) == true );
		BOOST_CHECK( manager.execute_string( "assert( fw.server ~= nil and fw.Server ~= nil )" ) == true );
		BOOST_CHECK( manager.execute_string( "assert( fw.world ~= nil and fw.World ~= nil )" ) == true );
	}

	// Trigger command.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_CHECK( manager.execute_string( "function foo( args, sender ) fw.test:set_value( \"yo\", args[1] .. args[2] .. tostring( sender ) ) end" ) );
		BOOST_CHECK( manager.execute_string( "fw.event:hook_command( \"test\", foo )" ) );

		std::vector<sf::String> args;
		args.push_back( sf::String( L"hello" ) );
		args.push_back( sf::String( L"world" ) );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK( manager.trigger_command( "test", args, 123 ) == true );
		BOOST_CHECK( manager.get_last_error().empty() == true );

		BOOST_CHECK( manager.execute_string( "assert( fw.test:find_value( \"yo\" ) == \"helloworld123\" )" ) );

		// Error handling.
		manager.clear();

		BOOST_REQUIRE( manager.execute_string( "function faulty() assert( false ) end" ) == true );
		BOOST_REQUIRE( manager.execute_string( "fw.event:hook_command( \"meow\", faulty )" ) == true );

		BOOST_CHECK( manager.trigger_command( "meow", args, 888 ) == false );
		BOOST_CHECK( manager.get_last_error().empty() == false );
	}

	// Trigger chat system event.
	{
		ScriptManager manager( server_gate, world_gate );
		bool result = false;

		BOOST_REQUIRE( result = manager.execute_file( DATA_DIRECTORY + std::string( "/scripts/chat.lua" ) ) );
		BOOST_REQUIRE( result == true );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK( manager.trigger_chat_event( sf::String( L"Hell\xF6" ), sf::String( L"Ch\xE4nnel" ), 1337 ) == true );
		BOOST_CHECK( manager.get_last_error().empty() == true );

		// Error handling.
		manager.clear();

		BOOST_REQUIRE( manager.execute_string( "function faulty() assert( false ) end" ) == true );
		BOOST_REQUIRE( manager.execute_string( "fw.event:hook_event( fw.Event.CHAT, faulty )" ) == true );

		BOOST_CHECK( manager.trigger_chat_event( sf::String(), sf::String(), 0 ) == false );
		BOOST_CHECK( manager.get_last_error().empty() == false );
	}

	// Trigger connect event.
	{
		ScriptManager manager( server_gate, world_gate );
		bool result = false;

		BOOST_REQUIRE( result = manager.execute_file( DATA_DIRECTORY + std::string( "/scripts/connect.lua" ) ) );
		BOOST_REQUIRE( result == true );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK( manager.trigger_connect_event( 1337 ) == true );
		BOOST_CHECK( manager.get_last_error().empty() == true );

		// Error handling.
		manager.clear();

		BOOST_REQUIRE( manager.execute_string( "function faulty() assert( false ) end" ) == true );
		BOOST_REQUIRE( manager.execute_string( "fw.event:hook_event( fw.Event.CONNECT, faulty )" ) == true );

		BOOST_CHECK( manager.trigger_connect_event( 1337 ) == false );
		BOOST_CHECK( manager.get_last_error().empty() == false );
	}

	// Trigger use class event.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_REQUIRE( manager.execute_string( "function on_use( entity_id, actor_id, client_id ) assert( entity_id == 11 and actor_id == 22 and client_id == 33 ); fw.test:set_value( \"use_ok\", \"yes\" ) end" ) );
		BOOST_REQUIRE( manager.execute_string( "fw.event:hook_event( fw.Event.USE, on_use )" ) == true );

		Class cls( FlexID::make( "some/class" ) );

		Entity object( cls );
		object.set_id( 11 );

		Entity actor( cls );
		actor.set_id( 22 );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK(
			manager.trigger_use_event(
				object,
				actor,
				33
			) == true
		);
		BOOST_CHECK( manager.get_last_error().empty() == true );

		BOOST_CHECK_NO_THROW( manager.execute_string( "assert( fw.test:find_value( \"use_ok\" ) == \"yes\" )" ) );
	}

	// Trigger block action class event.
	{
		ScriptManager manager( server_gate, world_gate );

		BOOST_REQUIRE( manager.execute_file( DATA_DIRECTORY + std::string( "/scripts/block_action.lua" ) ) == true );
		BOOST_REQUIRE( manager.execute_string( "fw.event:hook_event( fw.Event.BLOCK_ACTION, handler )" ) == true );

		Class cls( FlexID::make( "some/class" ) );
		Entity actor( cls );
		actor.set_id( 100 );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK(
			manager.trigger_block_action_event(
				lua::Event::BlockPosition( 1, 2, 3 ),
				lua::Event::BlockPosition( 4, 5, 6 ),
				true,
				actor,
				1337
			) == true
		);
	}
}
