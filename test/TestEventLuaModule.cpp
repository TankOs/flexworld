#include "Config.hpp"
#include "LuaUtils.hpp"

#include <FlexWorld/LuaModules/Server.hpp>
#include <FlexWorld/LuaModules/ServerGate.hpp>
#include <FlexWorld/LuaModules/Test.hpp>

#include <Diluculum/LuaState.hpp>
#include <boost/test/unit_test.hpp>
#include <cassert>

class ExampleServerGate : public flex::lua::ServerGate {
	public:
		const std::string& get_client_username( uint16_t client_id ) const {
			if( client_id != 4711 ) {
				throw std::runtime_error( "Invalid client ID." );
			}

			static const std::string h4x0r = "H4x0r";
			return h4x0r;
		}
};

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
}
