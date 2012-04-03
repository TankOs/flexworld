#include "Config.hpp"
#include "LuaUtils.hpp"
#include "ExampleServerGate.hpp"

#include <FlexWorld/LuaModules/Server.hpp>
#include <FlexWorld/LuaModules/Test.hpp>

#include <Diluculum/LuaState.hpp>
#include <boost/test/unit_test.hpp>
#include <cassert>

void setup_state_for_server( Diluculum::LuaState& state ) {
	state["flex"] = Diluculum::EmptyTable;

	flex::lua::Server::register_class( state["flex"]["Server"] );
	flex::lua::Test::register_class( state["flex"]["Test"] );
}

BOOST_AUTO_TEST_CASE( TestServerLuaModule ) {
	using namespace flex;

	// Initial state.
	{
		Diluculum::LuaState state;
		setup_state_for_server( state );

		// Valid ctor.
		{
			ExampleServerGate gate;
			lua::Server server( gate );
		}

		// Lua ctor.
		{
			BOOST_CHECK( check_error( "Not allowed to instantiate Server.", "foo = flex.Server.new()", state ) == true );
		}
	}

	// Register object.
	{
		Diluculum::LuaState state;
		setup_state_for_server( state );

		ExampleServerGate gate;
		lua::Server server( gate );
		server.register_object( state["flex"]["server"] );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.server ~= nil )" ) );
	}

	// Get client username.
	{
		Diluculum::LuaState state;
		setup_state_for_server( state );

		ExampleServerGate gate;
		lua::Server server( gate );
		server.register_object( state["flex"]["server"] );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.server:get_client_username( 4711 ) == \"H4x0r\" )" ) );
		BOOST_CHECK( check_error( "Invalid client ID.", "assert( flex.server:get_client_username( 1000 ) == \"H4x0r\" )", state ) == true );
	}

	// Get number of connected clients.
	{
		Diluculum::LuaState state;
		setup_state_for_server( state );

		ExampleServerGate gate;
		lua::Server server( gate );
		server.register_object( state["flex"]["server"] );

		BOOST_CHECK_NO_THROW( state.doString( "assert( flex.server:get_num_connected_clients() == 123 )" ) );
	}

	// Broadcast chat message.
	{
		Diluculum::LuaState state;
		setup_state_for_server( state );

		ExampleServerGate gate;
		lua::Server server( gate );
		server.register_object( state["flex"]["server"] );

		BOOST_CHECK_NO_THROW( state.doString( "flex.server:broadcast_chat_message( \"Message\", \"Channel\", \"Sender\" )" ) );

		// Give non-expected values.
		BOOST_CHECK( check_error( "Invalid message.","flex.server:broadcast_chat_message( \"Fuxx\", \"Channel\", \"Sender\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid channel.","flex.server:broadcast_chat_message( \"Message\", \"Fuxx\", \"Sender\" )", state ) == true );
		BOOST_CHECK( check_error( "Invalid sender.","flex.server:broadcast_chat_message( \"Message\", \"Channel\", \"Fuxx\" )", state ) == true );
	}

	// Call functions with wrong arguments.
	{
		Diluculum::LuaState state;
		setup_state_for_server( state );

		ExampleServerGate gate;
		lua::Server server( gate );
		server.register_object( state["flex"]["server"] );

		// get_client_username
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:get_client_username()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:get_client_username( 123, 123 )", state ) == true );

		BOOST_CHECK( check_error( "Expected number for client_id.", "flex.server:get_client_username( \"foo\" )", state ) == true );

		// get_num_connected_clients
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:get_num_connected_clients( 123 )", state ) == true );

		// broadcast_chat_message
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:broadcast_chat_message()", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:broadcast_chat_message( \"Message\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:broadcast_chat_message( \"Message\", \"Channel\" )", state ) == true );
		BOOST_CHECK( check_error( "Wrong number of arguments.", "flex.server:broadcast_chat_message( \"Message\", \"Channel\", \"Sender\", \"Too much\" )", state ) == true );

		BOOST_CHECK( check_error( "Expected string for message.", "flex.server:broadcast_chat_message( 123, \"Channel\", \"Sender\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected string for channel.", "flex.server:broadcast_chat_message( \"Message\", 123, \"Sender\" )", state ) == true );
		BOOST_CHECK( check_error( "Expected string for sender.", "flex.server:broadcast_chat_message( \"Message\", \"Channel\", 123 )", state ) == true );
	}
}
