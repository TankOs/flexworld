#include "LuaUtils.hpp"

#include <FlexWorld/LuaModules/Event.hpp>
#include <FlexWorld/LuaModules/Test.hpp>

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
	}
}
