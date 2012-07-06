#pragma once

#include <SFML/System/String.hpp>
#include <SFML/System/Vector3.hpp>
#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace Diluculum {
class LuaState;
}

namespace flex {
class Entity;

namespace lua {
class Test;
class Event;
class Server;
class ServerGate;
class World;
class WorldGate;
}

/** Lua script manager.
 */
class ScriptManager {
	public:
		/** Ctor.
		 * @param server_gate Server gate.
		 * @param world_gate World gate.
		 */
		ScriptManager( lua::ServerGate& server_gate, lua::WorldGate& world_gate );

		/** Dtor.
		 */
		~ScriptManager();

		/** Clear the WHOLE Lua state.
		 * All functions, variables etc. will be removed and a clean state will be
		 * relaunched.
		 */
		void clear();

		/** Execute file.
		 * @param path Path.
		 * @return true on success.
		 * @see get_last_error to retrieve last error.
		 */
		bool execute_file( const std::string& path );

		/** Execute string.
		 * @param code Code.
		 * @return true on success.
		 * @see get_last_error to retrieve last error.
		 */
		bool execute_string( const std::string& code );

		/** Get last error.
		 * @return Last error or empty if no error happened.
		 */
		const std::string& get_last_error() const;

		/** Trigger command.
		 * @param command Command (must be non-empty and valid).
		 * @param args Arguments.
		 * @param sender Sender client ID.
		 * @return false if error occured.
		 * @see is_valid_command
		 */
		bool trigger_command( const std::string& command, const std::vector<sf::String>& args, uint16_t sender );

		/** Trigger connect event.
		 * @param client_id Client ID.
		 * @return false if error occured.
		 */
		bool trigger_connect_event( uint16_t client_id );

		/** Trigger chat event.
		 * @param message Message.
		 * @param channel Channel.
		 * @param sender Sender.
		 * @return false if error occured.
		 */
		bool trigger_chat_event( const sf::String& message, const sf::String& channel, uint16_t sender );

		/** Trigger use event.
		 * @param entity Entity being used.
		 * @param actor Actor entity.
		 * @param client_id Client ID.
		 * @return false if error occured.
		 */
		bool trigger_use_event( const Entity& entity, const Entity& actor, uint16_t client_id );

		/** Trigger block action event.
		 * @param block_pos Block position of block the action is performed on.
		 * @param next_block_pos Block position of block's neighbour (depending on which face the player clicked on).
		 * @param primary true if primary action, otherwise secondary.
		 * @param actor Actor entity (either entity in hand or player entity itself if not holding anything).
		 * @param client_id Client ID.
		 * @return false if error occured.
		 */
		bool trigger_block_action_event(
			const sf::Vector3<uint32_t>& block_pos,
			const sf::Vector3<uint32_t>& next_block_pos,
			bool primary,
			const flex::Entity& actor,
			uint16_t client_id
		);

	private:
		std::string m_last_error;

		Diluculum::LuaState* m_state;
		lua::Test* m_test_module;
		lua::Event* m_event_module;

		lua::ServerGate& m_server_gate;
		lua::Server* m_server_module;

		lua::WorldGate& m_world_gate;
		lua::World* m_world_module;
};

}
