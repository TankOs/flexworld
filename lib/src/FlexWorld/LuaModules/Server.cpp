#include <FlexWorld/LuaModules/Server.hpp>
#include <FlexWorld/LuaModules/ServerGate.hpp>

#include <Diluculum/LuaWrappers.hpp>
#include <Diluculum/LuaExceptions.hpp>

#include <cstdint>

namespace flex {
namespace lua {

DILUCULUM_BEGIN_CLASS( Server )
	DILUCULUM_CLASS_METHOD( Server, get_client_username )
DILUCULUM_END_CLASS( Server )

void Server::register_class( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_CLASS( target, Server );
}

Server::Server( const Diluculum::LuaValueList& args ) {
	throw Diluculum::LuaError( "Not allowed to instantiate Server." );
}

Server::Server( ServerGate& gate ) :
	m_gate( &gate )
{
}

void Server::register_object( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_OBJECT( target, Server, *this );
}

Diluculum::LuaValueList Server::get_client_username( const Diluculum::LuaValueList& args ) const {
	if( args.size() != 1 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for client_id." );
	}

	// Extract values.
	uint16_t client_id = static_cast<uint16_t>( args[0].asNumber() );

	Diluculum::LuaValueList ret;

	// Try to fetch username.
	try {
		const std::string& username = m_gate->get_client_username( client_id );
		
		ret.push_back( username );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return ret;
}

}
}
