#include "Config.hpp"

#include <FlexWorld/ScriptManager.hpp>

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
}
