#include "LuaUtils.hpp"

#include <FlexWorld/LuaModules/Test.hpp>

#include <Diluculum/LuaState.hpp>
#include <boost/test/unit_test.hpp>

void setup_state( Diluculum::LuaState& state ) {
	state["fw"] = Diluculum::EmptyTable;

	fw::lua::Test::register_class( state["fw"]["Test"] );
}

BOOST_AUTO_TEST_CASE( TestTestLuaModule ) {
	using namespace fw;

	// Initial state.
	{
		lua::Test test;

		BOOST_CHECK( test.get_num_values()[0] == 0 );
	}

	// Set/find values.
	{
		Diluculum::LuaState state;
		setup_state( state );

		lua::Test test;
		test.register_object( state["fw"]["test"] );

		BOOST_CHECK_NO_THROW( state.doString( "fw.test:set_value( \"foo\", \"bar\" )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "fw.test:set_value( \"meh\", \"bleh\" )" ) );

		BOOST_CHECK( test.get_num_values()[0] == 2 );

		BOOST_CHECK_NO_THROW( state.doString( "assert( fw.test:find_value( \"foo\" ) == \"bar\" )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "assert( fw.test:find_value( \"meh\" ) == \"bleh\" )" ) );

		BOOST_CHECK_NO_THROW( state.doString( "assert( fw.test:find_value( \"meow\" ) == nil )" ) );
	}

	// Call functions with wrong arguments.
	{
		Diluculum::LuaState state;
		setup_state( state );

		lua::Test test;
		test.register_object( state["fw"]["test"] );

		BOOST_CHECK( check_error( "Wrong number of arguments.", "fw.test:set_value()", state ) );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "fw.test:set_value( \"abc\" )", state ) );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "fw.test:set_value( \"abc\", \"abc\", \"abc\" )", state ) );
		BOOST_CHECK( check_error( "Expected string for key.", "fw.test:set_value( 123, \"abc\" )", state ) );
		BOOST_CHECK( check_error( "Expected string for value.", "fw.test:set_value( \"abc\", 123 )", state ) );

		BOOST_CHECK( check_error( "Wrong number of arguments.", "fw.test:find_value()", state ) );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "fw.test:find_value( \"123\", \"123\" )", state ) );
		BOOST_CHECK( check_error( "Expected string for key.", "fw.test:find_value( 123 )", state ) );
	}
}
