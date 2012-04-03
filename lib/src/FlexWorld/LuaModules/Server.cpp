#include <FlexWorld/LuaModules/Server.hpp>
#include <FlexWorld/LuaModules/ServerGate.hpp>

#include <SFML/System/Utf.hpp>
#include <Diluculum/LuaWrappers.hpp>
#include <Diluculum/LuaExceptions.hpp>
#include <string>
#include <iterator>
#include <cstdint>

namespace flex {
namespace lua {

DILUCULUM_BEGIN_CLASS( Server )
	DILUCULUM_CLASS_METHOD( Server, get_client_username )
	DILUCULUM_CLASS_METHOD( Server, get_num_connected_clients )
	DILUCULUM_CLASS_METHOD( Server, broadcast_chat_message )
DILUCULUM_END_CLASS( Server )

void Server::register_class( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_CLASS( target, Server );
}

Server::Server( const Diluculum::LuaValueList& /*args*/ ) {
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

Diluculum::LuaValueList Server::get_num_connected_clients( const Diluculum::LuaValueList& args ) const {
	if( args.size() != 0 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	Diluculum::LuaValueList ret;

	try {
		ret.push_back( static_cast<int>( m_gate->get_num_connected_clients() ) );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return ret;
}

Diluculum::LuaValueList Server::broadcast_chat_message( const Diluculum::LuaValueList& args ) {
	if( args.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for message." );
	}

	if( args[1].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for channel." );
	}

	if( args[2].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for sender." );
	}

	std::string message_u8 = args[0].asString();
	std::string channel_u8 = args[1].asString();
	std::string sender_u8 = args[2].asString();

	if( message_u8.empty() ) {
		throw Diluculum::LuaError( "Invalid message." );
	}

	if( channel_u8.empty() ) {
		throw Diluculum::LuaError( "Invalid channel." );
	}

	if( sender_u8.empty() ) {
		throw Diluculum::LuaError( "Invalid sender." );
	}

	// Check maximum size after conversion.

	// Convert UTF-8 to UTF-32.
	std::basic_string<sf::Uint32> message_u32;
	std::basic_string<sf::Uint32> channel_u32;
	std::basic_string<sf::Uint32> sender_u32;

	sf::Utf8::toUtf32( message_u8.begin(), message_u8.end(), std::back_inserter( message_u32 ) );
	sf::Utf8::toUtf32( channel_u8.begin(), channel_u8.end(), std::back_inserter( channel_u32 ) );
	sf::Utf8::toUtf32( sender_u8.begin(), sender_u8.end(), std::back_inserter( sender_u32 ) );

	try {
		m_gate->broadcast_chat_message( sf::String( message_u32 ), sf::String( channel_u32 ), sf::String( sender_u32 ) );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return Diluculum::LuaValueList();
}

}
}
