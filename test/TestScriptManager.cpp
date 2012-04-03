#include "Config.hpp"

#include <FlexWorld/ScriptManager.hpp>

#include <SFML/System/String.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestScriptManager ) {
	using namespace flex;

	// Initial state.
	{
		ScriptManager manager;

	}

	// Execute files.
	{
		ScriptManager manager;

		BOOST_CHECK( manager.execute_file( DATA_DIRECTORY + "/scripts/some_funcs.lua" ) == true );
	}

	// Execute string.
	{
		ScriptManager manager;

		BOOST_CHECK( manager.execute_string( "a = 5" ) == true );
	}

	// Verify last error is reset.
	{
		ScriptManager manager;

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
		ScriptManager manager;

		BOOST_CHECK( manager.execute_string( "a = 5" ) == true );
		BOOST_CHECK( manager.execute_string( "assert( a == 5 )" ) == true );

		manager.clear();

		BOOST_CHECK( manager.execute_string( "assert( a == nil )" ) == true );
	}

	// Make sure modules are present.
	{
		ScriptManager manager;

		BOOST_CHECK( manager.execute_string( "assert( flex.test ~= nil and flex.Test ~= nil )" ) == true );
		BOOST_CHECK( manager.execute_string( "assert( flex.event ~= nil and flex.Event ~= nil )" ) == true );
	}

	// Trigger command.
	{
		ScriptManager manager;

		BOOST_REQUIRE_NO_THROW( manager.execute_string( "function foo( args ) flex.test:set_value( \"yo\", args[1] .. args[2] ) end" ) );
		BOOST_REQUIRE_NO_THROW( manager.execute_string( "flex.event:hook_command( \"test\", foo )" ) );

		std::vector<sf::String> args;
		args.push_back( sf::String( L"hello" ) );
		args.push_back( sf::String( L"world" ) );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK( manager.trigger_command( "test", args ) == true );
		BOOST_CHECK( manager.get_last_error().empty() == true );

		BOOST_CHECK_NO_THROW( manager.execute_string( "assert( flex.test:get_value( \"yo\" ) == \"halloworld\" )" ) );

		// Error handling.
		manager.clear();

		BOOST_REQUIRE( manager.execute_string( "function faulty() assert( false ) end" ) == true );
		BOOST_REQUIRE( manager.execute_string( "flex.event:hook_command( \"meow\", faulty )" ) == true );

		BOOST_CHECK( manager.trigger_command( "meow", args ) == false );
		BOOST_CHECK( manager.get_last_error().empty() == false );
	}

	// Trigger chat system event.
	{
		ScriptManager manager;
		bool result = false;


		BOOST_REQUIRE_NO_THROW( result = manager.execute_file( DATA_DIRECTORY + std::string( "/scripts/chat.lua" ) ) );
		BOOST_REQUIRE( result == true );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK( manager.trigger_chat_system_event( sf::String( L"Hell\xF6" ), sf::String( L"Ch\xE4nnel" ), 1337 ) == true );
		BOOST_CHECK( manager.get_last_error().empty() == true );

		// Error handling.
		manager.clear();

		BOOST_REQUIRE( manager.execute_string( "function faulty() assert( false ) end" ) == true );
		BOOST_REQUIRE( manager.execute_string( "flex.event:hook_system_event( flex.Event.System.CHAT, faulty )" ) == true );

		BOOST_CHECK( manager.trigger_chat_system_event( sf::String(), sf::String(), 0 ) == false );
		BOOST_CHECK( manager.get_last_error().empty() == false );
	}

	// Trigger connect system event.
	{
		ScriptManager manager;
		bool result = false;

		BOOST_REQUIRE_NO_THROW( result = manager.execute_file( DATA_DIRECTORY + std::string( "/scripts/connect.lua" ) ) );
		BOOST_REQUIRE( result == true );

		BOOST_CHECK( manager.get_last_error().empty() == true );
		BOOST_CHECK( manager.trigger_connect_system_event( 1337 ) == true );
		BOOST_CHECK( manager.get_last_error().empty() == true );

		// Error handling.
		manager.clear();

		BOOST_REQUIRE( manager.execute_string( "function faulty() assert( false ) end" ) == true );
		BOOST_REQUIRE( manager.execute_string( "flex.event:hook_system_event( flex.Event.System.CONNECT, faulty )" ) == true );

		BOOST_CHECK( manager.trigger_connect_system_event( 1337 ) == false );
		BOOST_CHECK( manager.get_last_error().empty() == false );
	}
}
