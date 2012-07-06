#include "Config.hpp"
#include "LuaUtils.hpp"
#include "ExceptionChecker.hpp"

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

			BOOST_CHECK( event.get_num_event_hooks() == 0 );
			BOOST_CHECK( event.get_num_command_hooks() == 0 );
		}

		// Lua ctor.
		{
			BOOST_CHECK( check_error( "Not allowed to instantiate Event.", "foo = flex.Event.new()", state ) == true );
		}
	}

	// Hook events.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Event event;
		event.register_object( state["flex"]["event"] );

		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_event( flex.Event.CONNECT, function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_event( flex.Event.CHAT, function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_event( flex.Event.USE, function() end )" ) );
		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_event( flex.Event.BLOCK_ACTION, function() end )" ) );

		BOOST_CHECK( event.get_num_event_hooks() == 4 );
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

	// Trigger events.
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

		BOOST_CHECK_NO_THROW( state.doString( "flex.event:hook_event( flex.Event.CONNECT, on_connect )" ) );
		event.trigger_connect_event( 1337, state );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"connect\" ) == \"called1337\" )" ) );
	}

	// Trigger chat event.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Test test;
		lua::Event event;

		test.register_object( state["flex"]["test"] );
		event.register_object( state["flex"]["event"] );

		// Load script.
		BOOST_CHECK_NO_THROW( state.doFile( DATA_DIRECTORY + std::string( "/scripts/chat.lua" ) ) );

		// Trigger.
		BOOST_CHECK_NO_THROW( event.trigger_chat_event( sf::String( L"Hell\xF6" ), sf::String( L"Ch\xE4nnel" ), 1337, state ) );
	}

	// Trigger class events.
	{
		// Create test data.
		Class cls_actor( FlexID::make( "class/actor" ) );
		Class cls_a( FlexID::make( "class/a" ) );
		Class cls_b( FlexID::make( "class/b" ) );

		Entity ent_actor( cls_actor );
		Entity ent_a( cls_a );
		Entity ent_b( cls_b );

		ent_actor.set_id( 1337 );
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
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"use\" ) == nil )" ) );

		event.trigger_use_event( ent_a, ent_actor, 111, state );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"use\" ) == \"100,1337,111\" )" ) );

		event.trigger_use_event( ent_b, ent_actor, 222, state );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"use\" ) == \"200,1337,222\" )" ) );

		// Trigger block action event.
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"block_action\" ) == nil )" ) );

		event.trigger_block_action_event(
			lua::Event::BlockPosition( 1, 2, 3 ),
			lua::Event::BlockPosition( 4, 5, 6 ),
			true,
			ent_a,
			1337,
			state
		);

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"block_action\" ) == \"called\" )" ) );
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
		event.trigger_command( "hello_world", args, 1337, state );
		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.test:find_value( \"hello_world\" ) == \"hell\xC3\xA4 hell\xC3\xB6\" )" ) );
	}

	// Call functions with invalid arguments.
	{
		Diluculum::LuaState state;
		setup_state_for_event( state );

		lua::Event event;
		event.register_object( state["flex"]["event"] );

		// hook_event
		std::string num_events;

		{
			std::stringstream sstr;
			sstr << lua::Event::NUM_EVENTS;
			num_events = sstr.str();
		}

		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_event()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_event( flex.Event.CONNECT )", state ) == true );

		BOOST_CHECK( check_error( "Expected number for event.", "flex.event:hook_event( \"meh\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Expected function for callback.", "flex.event:hook_event( flex.Event.CONNECT, 123 )", state ) == true );

		BOOST_CHECK( check_error( "Invalid event ID.", "flex.event:hook_event( " + num_events + ", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Invalid event ID.", "flex.event:hook_event( -1, function() end )", state ) == true );

		// hook_command
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_command()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.event:hook_command( \"meh\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for command.", "flex.event:hook_command( 123, function() end )", state ) == true );
		BOOST_CHECK( check_error( "Expected function for callback.", "flex.event:hook_command( \"meh\", 123 )", state ) == true );

		BOOST_CHECK( check_error( "Invalid command.", "flex.event:hook_command( \"\", function() end )", state ) == true );
		BOOST_CHECK( check_error( "Invalid command.", "flex.event:hook_command( \"$§%%$&=\", function() end )", state ) == true );
	}
}
