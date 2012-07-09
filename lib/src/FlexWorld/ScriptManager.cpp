#include <FlexWorld/ScriptManager.hpp>
#include <FlexWorld/LuaModules/Test.hpp>
#include <FlexWorld/LuaModules/Event.hpp>
#include <FlexWorld/LuaModules/Server.hpp>
#include <FlexWorld/LuaModules/World.hpp>
#include <FlexWorld/Log.hpp>

#include <Diluculum/LuaState.hpp>

namespace fw {

ScriptManager::ScriptManager( lua::ServerGate& server_gate, lua::WorldGate& world_gate ) :
	m_state( nullptr ),
	m_test_module( nullptr ),
	m_event_module( nullptr ),
	m_server_gate( server_gate ),
	m_server_module( nullptr ),
	m_world_gate( world_gate ),
	m_world_module( nullptr )
{
	// Initialize.
	clear();
}

ScriptManager::~ScriptManager() {
	delete m_world_module;
	delete m_server_module;
	delete m_event_module;
	delete m_test_module;
	delete m_state;
}

void ScriptManager::clear() {
	m_last_error = "";
	
	// Clean up modules and state.
	delete m_event_module;
	delete m_test_module;
	delete m_server_module;
	delete m_world_module;

	delete m_state;

	// Recreate state.
	m_state = new Diluculum::LuaState;

	// Setup state.
	(*m_state)["fw"] = Diluculum::EmptyTable;

	// Create modules.
	m_test_module = new lua::Test;
	m_event_module = new lua::Event;
	m_server_module = new lua::Server( m_server_gate );
	m_world_module = new lua::World( m_world_gate );

	// Setup modules.
	lua::Test::register_class( (*m_state)["fw"]["Test"] );
	lua::Event::register_class( (*m_state)["fw"]["Event"] );
	lua::Server::register_class( (*m_state)["fw"]["Server"] );
	lua::World::register_class( (*m_state)["fw"]["World"] );

	m_test_module->register_object( (*m_state)["fw"]["test"] );
	m_event_module->register_object( (*m_state)["fw"]["event"] );
	m_server_module->register_object( (*m_state)["fw"]["server"] );
	m_world_module->register_object( (*m_state)["fw"]["world"] );
}

bool ScriptManager::execute_file( const std::string& path ) {
	m_last_error = "";

	bool result = false;

	try {
		m_state->doFile( path );
		result = true;
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
		result = false;
	}

	return result;
}

bool ScriptManager::execute_string( const std::string& code ) {
	m_last_error = "";

	bool result = false;

	try {
		m_state->doString( code );
		result = true;
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
	}

	return result;
}

const std::string& ScriptManager::get_last_error() const {
	return m_last_error;
}

bool ScriptManager::trigger_command( const std::string& command, const std::vector<sf::String>& args, uint16_t sender ) {
	m_last_error = "";

	try {
		m_event_module->trigger_command( command, args, sender, *m_state );
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
		Log::Logger( Log::ERR ) << "Command error: " << m_last_error << Log::endl;

		return false;
	}

	return true;
}

bool ScriptManager::trigger_chat_event( const sf::String& message, const sf::String& channel, uint16_t sender ) {
	m_last_error = "";

	try {
		m_event_module->trigger_chat_event( message, channel, sender, *m_state );
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
		Log::Logger( Log::ERR ) << "CHAT event error: " << m_last_error << Log::endl;

		return false;
	}

	return true;
}

bool ScriptManager::trigger_connect_event( uint16_t client_id ) {
	m_last_error = "";

	try {
		m_event_module->trigger_connect_event( client_id, *m_state );
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
		Log::Logger( Log::ERR ) << "CONNECT event error: " << m_last_error << Log::endl;

		return false;
	}

	return true;
}

bool ScriptManager::trigger_use_event( const Entity& entity, const Entity& actor, uint16_t client_id ) {
	m_last_error.clear();

	try {
		m_event_module->trigger_use_event( entity, actor, client_id, *m_state );
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
		Log::Logger( Log::ERR ) << "USE event error: " << m_last_error << Log::endl;

		return false;
	}

	return true;
}

bool ScriptManager::trigger_block_action_event(
	const sf::Vector3<uint32_t>& block_pos,
	const sf::Vector3<uint32_t>& next_block_pos,
	bool primary,
	const fw::Entity& actor,
	uint16_t client_id
) {
	m_last_error.clear();

	try {
		m_event_module->trigger_block_action_event( block_pos, next_block_pos, primary, actor, client_id, *m_state );
	}
	catch( const Diluculum::LuaError& e ) {
		m_last_error = e.what();
		Log::Logger( Log::ERR ) << "BLOCK_ACTION event error: " << m_last_error << Log::endl;

		return false;
	}

	return true;
}

}
