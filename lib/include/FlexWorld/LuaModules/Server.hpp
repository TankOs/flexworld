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

namespace fw {
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

		/** Get number of connected clients (Lua function).
		 * @param args Ignored.
		 * @return num:number
		 */
		Diluculum::LuaValueList get_num_connected_clients( const Diluculum::LuaValueList& args ) const;

		/** Broadcast a chat message.
		 * @param args message:string, channel:string, sender:string
		 * @return nil
		 */
		Diluculum::LuaValueList broadcast_chat_message( const Diluculum::LuaValueList& args );

		/** Get a client's entity ID.
		 * @param args client_id:number
		 * @return entity_id:number
		 */
		Diluculum::LuaValueList get_client_entity_id( const Diluculum::LuaValueList& args );

	private:
		ServerGate* m_gate;
};

}
}
