#pragma once

#include <Diluculum/LuaVariable.hpp>
#include <Diluculum/LuaFunction.hpp>

/*#include <SFML/System/String.hpp>
#include <map>
#include <vector>
#include <cstdint>*/

namespace Diluculum {
class LuaState;
}

namespace flex {
namespace lua {

class ServerGate;

/** Server Lua module.
 */
class Server {
	public:
		/** Register class with state.
		 * @param target Target variable.
		 */
		static void register_class( Diluculum::LuaVariable target );

		/** Lua ctor.
		 * Not allowed to call.
		 * @param args Arguments (ignored).
		 * @throws Diluculum::LuaError
		 */
		Server( const Diluculum::LuaValueList& args );

		/** Ctor.
		 * Used by C++.
		 * @param gate Gate.
		 */
		Server( ServerGate& gate );

		/** Register object.
		 * @param target Target variable.
		 */
		void register_object( Diluculum::LuaVariable target );

		/** Get username of client (Lua function).
		 * @param args client_id:number
		 * @return username:string
		 */
		Diluculum::LuaValueList get_client_username( const Diluculum::LuaValueList& args ) const;

	private:
		ServerGate* m_gate;
};

}
}
