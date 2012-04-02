#pragma once

#include <SFML/System/String.hpp>
#include <string>
#include <memory>
#include <vector>

namespace Diluculum {
class LuaState;
}

namespace flex {

namespace lua {
class Test;
class Event;
}

/** Lua script manager.
 */
class ScriptManager {
	public:
		/** Ctor.
		 */
		ScriptManager();

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
		 * @see is_valid_command
		 */
		void trigger_command( const std::string& command, const std::vector<sf::String>& args );

		/** Trigger chat system event.
		 * @param message Message.
		 * @param channel Channel.
		 * @param sender Sender.
		 */
		void trigger_chat_system_event( const sf::String& message, const sf::String& channel, uint16_t sender );

	private:
		std::string m_last_error;

		Diluculum::LuaState* m_state;
		lua::Test* m_test_module;
		lua::Event* m_event_module;
};

}
