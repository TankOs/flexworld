#include "Config.hpp"
#include "LuaUtils.hpp"

#include <FlexWorld/LuaModules/Event.hpp>
#include <FlexWorld/LuaModules/Test.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/Entity.hpp>

#include <Diluculum/LuaState.hpp>
#include <boost/test/unit_test.hpp>

void setup_state_for_event( Diluculum::LuaState& state ) {
	state["flex"] = Diluculum::EmptyTable;

	flex::lua::Event::register_class( state["flex"]["Event"] );
	flex::lua::Test::register_class( state["flex"]["Test"] );
}

BOOST_AUTO_TEST_CASE( TestEventLuaModule ) {
	using namespace flex;

	// Initial state.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		// Valid ctor.
		{
			lua::Event event;

			BOOST_CHECK( event.get_num_system_hooks() == 0 );
			BOOST_CHECK( event.get_num_class_hooks() == 0 );
		}

		// Lua ctor.
		{
			BOOST_CHECK( check_error( "Not allowed to instantiate Event.", "foo = flex.Event.new()", state ) == true );
		}
	}

	// Hook system events.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Event event;
		event.register_object( state["flex"]["event"] );

		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_system_event( flex.Event.System.CONNECT, function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_system_event( flex.Event.System.DISCONNECT, function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_system_event( flex.Event.System.UNLOAD, function() end )" ) );

		BOOST_CHECK( event.get_num_system_hooks() == 3 );
	}

	// Hook class events.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Event event;
		event.register_object( state["flex"]["event"] );

		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_class_event( flex.Event.Class.USE, \"some/class\", function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_class_event( flex.Event.Class.PRIMARY_ACTION, \"some/class\", function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_class_event( flex.Event.Class.SECONDARY_ACTION, \"some/class\", function() end )" ) );

		BOOST_CHECK( event.get_num_class_hooks() == 3 );
	}

	// Hook commands.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Event event;
		event.register_object( state["flex"]["event"] );

		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_command( \"hello_world\", function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_command( \"hello_flex\", function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_command( \"hello_myself\", function() end )" ) );

		BOOST_CHECK( event.get_num_command_hooks() == 3 );
	}

	// Trigger system events.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Test test;
		lua::Event event;

		test.register_object( state["flex"]["test"] );
		event.register_object( state["flex"]["event"] );

		// Connect event.
		BOOST_CHECK_NO_THROW( state.doString( "function on_connect( client_id ) flex.test:set_value( \"connect\", \"called\" .. tostring( client_id ) ) end" ) );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"connect\" ) == nil )" ) );

		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect )" ) );
		event.trigger_connect_system_event( 1337, state );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"connect\" ) == \"called1337\" )" ) );
	}

	// Trigger class events.
	{
		// Create test data.
		Class cls_a( FlexID::make( "class/a" ) );
		Class cls_b( FlexID::make( "class/b" ) );

		Entity ent_a( cls_a );
		Entity ent_b( cls_b );

		ent_a.set_id( 100 );
		ent_b.set_id( 200 );

		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Test test;
		lua::Event event;

		test.register_object( state["flex"]["test"] );
		event.register_object( state["flex"]["event"] );

		// Load test environment.
		BOOST_CHECK_NO_THROW( state.doFile( DATA_DIRECTORY + std::string( "/scripts/class_events.lua" ) ) );

		// Trigger use event.
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"a_use\" ) == nil )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"b_use\" ) == nil )" ) );

		event.trigger_use_class_event( cls_a, ent_a, ent_b, state );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"a_use\" ) == \"100,200\" )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"b_use\" ) == nil )" ) );

		event.trigger_use_class_event( cls_b, ent_b, ent_a, state );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"a_use\" ) == \"100,200\" )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"b_use\" ) == \"200,100\" )" ) );
	}

	// Trigger commands.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Test test;
		lua::Event event;

		test.register_object( state["flex"]["test"] );
		event.register_object( state["flex"]["event"] );

		// Load test environment.
		BOOST_CHECK_NO_THROW( state.doFile( DATA_DIRECTORY + "/scripts/commands.lua" ) );

		// hello_world command.
		std::vector<sf::String> args;
		args.push_back( sf::String( L"hell\xE4" ) );
		args.push_back( sf::String( L"hell\xF6" ) );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"hello_world\" ) == nil )" ) );
		event.trigger_command( "hello_world", args, state );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"hello_world\" ) == \"hell\xC3\xA4 hell\xC3\xB6\" )" ) );
	}

	// Call functions with invalid arguments.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Event event;
		event.register_object( state["flex"]["event"] );

		// hook_system_event
		std::string num_system_events;
		std::string num_class_events;

		{
			std::stringstream sstr;
			sstr << lua::Event::NUM_SYSTEM_EVENTS;
			num_system_events = sstr.str();
		}
		{
			std::stringstream sstr;
			sstr << lua::Event::NUM_CLASS_EVENTS;
			num_class_events = sstr.str();
		}

		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_system_event()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_system_event( flex.Event.System.CONNECT )", state ) == true );

		BOOST_CHECK( check_error( "Expected number for event.", "flex.event:hook_system_event( \"meh\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Expected function for callback.", "flex.event:hook_system_event( flex.Event.System.CONNECT, 123 )", state ) == true );

		BOOST_CHECK( check_error( "Invalid system event ID.", "flex.event:hook_system_event( " + num_system_events + ", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Invalid system event ID.", "flex.event:hook_system_event( -1, function() end )", state ) == true );

		// hook_class_event
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_class_event()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_class_event( flex.Event.Class.USE )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_class_event( flex.Event.Class.USE, \"meh/bleh\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected number for event.", "flex.event:hook_class_event( \"meh\", \"meh/bleh\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Expected string for class.", "flex.event:hook_class_event( flex.Event.Class.USE, 123, function() end )", state ) == true );
		BOOST_CHECK( check_error( "Expected function for callback.", "flex.event:hook_class_event( flex.Event.Class.USE, \"meh/bleh\", 123 )", state ) == true );

		BOOST_CHECK( check_error( "Invalid class event ID.", "flex.event:hook_class_event( " + num_class_events + ", \"meh/bleh\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class event ID.", "flex.event:hook_class_event( -1, \"meh/bleh\", function() end )", state ) == true );

		BOOST_CHECK( check_error( "Invalid class ID.", "flex.event:hook_class_event( flex.Event.Class.USE, \"meh\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Invalid class ID.", "flex.event:hook_class_event( flex.Event.Class.USE, \"\", function() end )", state ) == true );

		// hook_command
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_command()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_command( \"meh\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for command.", "flex.event:hook_command( 123, function() end )", state ) == true );
		BOOST_CHECK( check_error( "Expected function for callback.", "flex.event:hook_command( \"meh\", 123 )", state ) == true );

		BOOST_CHECK( check_error( "Invalid command.", "flex.event:hook_command( \"\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Invalid command.", "flex.event:hook_command( \"$§%%$&=\", function() end )", state ) == true );
	}
}
