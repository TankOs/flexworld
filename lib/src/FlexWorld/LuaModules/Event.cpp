#include <FlexWorld/LuaModules/Event.hpp>
#include <FlexWorld/FlexID.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

#include <SFML/System/Utf.hpp>
#include <Diluculum/LuaState.hpp>
#include <Diluculum/LuaWrappers.hpp>
#include <iterator>
#include <iostream> // XXX TODO

namespace flex {
namespace lua {

DILUCULUM_BEGIN_CLASS( Event )
	DILUCULUM_CLASS_METHOD( Event, hook_system_event )
	DILUCULUM_CLASS_METHOD( Event, hook_class_event )
	DILUCULUM_CLASS_METHOD( Event, hook_command )
DILUCULUM_END_CLASS( Event )

void Event::register_class( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_CLASS( target, Event );

	// System events.
	target["System"] = Diluculum::EmptyTable;
	target["System"]["CONNECT"] = CONNECT_EVENT;
	target["System"]["CHAT"] = CHAT_EVENT;

	// Class events.
	target["Class"] = Diluculum::EmptyTable;
	target["Class"]["USE"] = USE_EVENT;
	target["Class"]["BLOCK_ACTION"] = BLOCK_ACTION_EVENT;
}

bool Event::is_valid_command( const std::string& command ) {
	if( command.empty() ) {
		return false;
	}

	for( std::size_t ch_idx = 0; ch_idx < command.size(); ++ch_idx ) {
		char ch = command[ch_idx];

		if(
			(ch < 'a' || ch > 'z') &&
			(ch < 'A' || ch > 'Z') &&
			(ch < '0' || ch > '9') &&
			ch != '_'
		) {
			return false;
		}
	}

	return true;
}

Event::Event( const Diluculum::LuaValueList& /*args*/ ) {
	throw Diluculum::LuaError( "Not allowed to instantiate Event." );
}

Event::Event() :
	m_system_functions( NUM_SYSTEM_EVENTS ),
	m_class_functions( NUM_CLASS_EVENTS )
{
}

void Event::register_object( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_OBJECT( target, Event, *this );
}

std::size_t Event::get_num_system_hooks() const {
	std::size_t num = 0;

	for( std::size_t idx = 0; idx < m_system_functions.size(); ++idx ) {
		num += m_system_functions[idx].size();
	}

	return num;
}

std::size_t Event::get_num_class_hooks() const {
	std::size_t num = 0;

	for( std::size_t idx = 0; idx < m_class_functions.size(); ++idx ) {
		num += m_class_functions[idx].size();
	}

	return num;
}

std::size_t Event::get_num_command_hooks() const {
	return m_command_functions.size();
}

Diluculum::LuaValueList Event::hook_system_event( const Diluculum::LuaValueList& args ) {
	if( args.size() != 2 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for event." );
	}

	if( args[1].type() != LUA_TFUNCTION ) {
		throw Diluculum::LuaError( "Expected function for callback." );
	}

	int event_id = static_cast<int>( args[0].asInteger() );

	if( event_id < 0 || event_id >= NUM_SYSTEM_EVENTS ) {
		throw Diluculum::LuaError( "Invalid system event ID." );
	}

	m_system_functions[event_id].push_back( args[1].asFunction() );

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList Event::hook_class_event( const Diluculum::LuaValueList& args ) {
	if( args.size() != 2 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for event." );
	}

	if( args[1].type() != LUA_TFUNCTION ) {
		throw Diluculum::LuaError( "Expected function for callback." );
	}

	int event_id = static_cast<int>( args[0].asInteger() );

	if( event_id < 0 || event_id >= NUM_CLASS_EVENTS ) {
		throw Diluculum::LuaError( "Invalid class event ID." );
	}

	m_class_functions[event_id].push_back( args[1].asFunction() );

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList Event::hook_command( const Diluculum::LuaValueList& args ) {
	if( args.size() != 2 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for command." );
	}

	if( args[1].type() != LUA_TFUNCTION ) {
		throw Diluculum::LuaError( "Expected function for callback." );
	}

	// Check for valid command.
	std::string command = args[0].asString();

	if( !is_valid_command( command ) ) {
		throw Diluculum::LuaError( "Invalid command." );
	}

	m_command_functions[command] = args[1].asFunction();

	return Diluculum::LuaValueList();
}

void Event::trigger_connect_system_event( uint16_t client_id, Diluculum::LuaState& state ) {
	Diluculum::LuaValueList args;
	args.push_back( client_id );

	call_system_event_callbacks( CONNECT_EVENT, args, state );
}

void Event::trigger_chat_system_event( const sf::String& message, const sf::String& channel, uint16_t sender, Diluculum::LuaState& state ) {
	// Convert UTF-32 to UTF-8.
	std::string message_u8;
	std::string channel_u8;

	sf::Utf32::toUtf8( message.begin(), message.end(), std::back_inserter( message_u8 ) );
	sf::Utf32::toUtf8( channel.begin(), channel.end(), std::back_inserter( channel_u8 ) );

	// Build Lua argument list.
	Diluculum::LuaValueList args;
	args.push_back( message_u8 );
	args.push_back( channel_u8 );
	args.push_back( sender );

	call_system_event_callbacks( CHAT_EVENT, args, state );
}

void Event::call_system_event_callbacks( SystemEvent event, const Diluculum::LuaValueList& args, Diluculum::LuaState& state ) {
	// Cache.
	FunctionArray& funcs = m_system_functions[event];
	std::size_t num = funcs.size();
	static const std::string CHUNK_NAME = "FW System Event";

	// Call Lua functions.
	for( std::size_t idx = 0; idx < num; ++idx ) {
		state.call( funcs[idx], args, CHUNK_NAME );
	}
}

void Event::call_class_event_callbacks( ClassEvent event, const Diluculum::LuaValueList& args, Diluculum::LuaState& state ) {
	// Cache.
	FunctionArray& funcs = m_class_functions[event];
	std::size_t num = funcs.size();
	static const std::string CHUNK_NAME = "FW Class Event";

	// Call Lua functions.
	for( std::size_t idx = 0; idx < num; ++idx ) {
		state.call( funcs[idx], args, CHUNK_NAME );
	}
}

void Event::trigger_use_class_event( const Entity& entity, const Entity& actor, uint16_t client_id, Diluculum::LuaState& state ) {
	Diluculum::LuaValueList args;

	args.push_back( entity.get_id() );
	args.push_back( actor.get_id() );
	args.push_back( client_id );

	call_class_event_callbacks( USE_EVENT, args, state );
}

void Event::trigger_command( const std::string& command, const std::vector<sf::String>& args, uint16_t sender, Diluculum::LuaState& state ) {
	assert( is_valid_command( command ) == true );

	// Look for functions.
	CommandFunctionMap::iterator cmd_iter = m_command_functions.find( command );

	if( cmd_iter == m_command_functions.end() ) { // None found, cancel.
		return;
	}

	// Build token list.
	Diluculum::LuaValue tokens = Diluculum::EmptyTable;

	for( std::size_t arg_idx = 0; arg_idx < args.size(); ++arg_idx ) {
		// Convert to UTF-8.
		std::string u_string;
		sf::Utf32::toUtf8( args[arg_idx].begin(), args[arg_idx].end(), std::back_inserter( u_string ) );

		tokens[arg_idx + 1] = u_string;
	}

	// Build arguments for call.
	Diluculum::LuaValueList call_args;

	call_args.push_back( tokens );
	call_args.push_back( static_cast<int>( sender ) );

	// Call command callback.
	state.call( cmd_iter->second, call_args, command );
}

void Event::trigger_block_action_class_event(
	const BlockPosition& block_pos,
	const BlockPosition& next_block_pos,
	bool primary,
	const flex::Entity& actor,
	uint16_t client_id,
	Diluculum::LuaState& state
) {
	// Build block position table.
	Diluculum::LuaValue block_pos_table = Diluculum::EmptyTable;
	block_pos_table[1] = block_pos.x;
	block_pos_table[2] = block_pos.y;
	block_pos_table[3] = block_pos.z;

	// Build next block position table.
	Diluculum::LuaValue next_block_pos_table = Diluculum::EmptyTable;
	next_block_pos_table[1] = next_block_pos.x;
	next_block_pos_table[2] = next_block_pos.y;
	next_block_pos_table[3] = next_block_pos.z;

	// Build argument list.
	Diluculum::LuaValueList args;

	args.push_back( block_pos_table );
	args.push_back( next_block_pos_table );
	args.push_back( primary );
	args.push_back( actor.get_id() );
	args.push_back( client_id );

	call_class_event_callbacks( BLOCK_ACTION_EVENT, args, state );
}

}
}
