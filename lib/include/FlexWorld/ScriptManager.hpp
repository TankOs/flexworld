#pragma once

#include <memory>

namespace Diluculum {
class LuaState;
}

namespace flex {

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

	private:
		Diluculum::LuaState* m_state;
};

}
