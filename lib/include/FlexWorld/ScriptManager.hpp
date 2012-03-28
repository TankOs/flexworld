#pragma once

#include <string>
#include <memory>

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

	private:
		std::string m_last_error;

		Diluculum::LuaState* m_state;
		lua::Test* m_test_module;
		lua::Event* m_event_module;
};

}
