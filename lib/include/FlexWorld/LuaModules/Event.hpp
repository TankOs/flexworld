#pragma once

#include <Diluculum/LuaVariable.hpp>
#include <Diluculum/LuaFunction.hpp>

#include <map>
#include <vector>

namespace flex {
namespace lua {

/** Event Lua module.
 */
class Event {
	public:
		/** System event.
		 */
		enum SystemEvent {
			CONNECT_EVENT = 0, ///< Client connected.
			DISCONNECT_EVENT, ///< Client disconnected.
			LOGIN_EVENT, ///< Client logged in.
			LOGIN_FAIL_EVENT, ///< Client failed to login.
			CHAT_EVENT, ///< Chat message.
			UNLOAD_EVENT, ///< Script gets unloaded.
			NUM_SYSTEM_EVENTS ///< Number of system events.
		};

		/** Class event.
		 */
		enum ClassEvent {
			USE_EVENT = 0, ///< Entity used object.
			TAKE_EVENT, ///< Entity took object.
			DROP_EVENT, ///< Entity dropped object.
			PRIMARY_ACTION_EVENT, ///< Primary action triggered on object.
			SECONDARY_ACTION_EVENT, ///< Secondary action triggered on object.
			CREATE_EVENT, ///< Object created.
			DESTROY_EVENT, ///< Object destroyed.
			NUM_CLASS_EVENTS ///< Number of class events.
		};

		/** Register class with state.
		 * @param target Target variable.
		 */
		static void register_class( Diluculum::LuaVariable target );

		/** Lua ctor.
		 * Not allowed to call.
		 * @param args Arguments (ignored).
		 * @throws Diluculum::LuaError
		 */
		Event( const Diluculum::LuaValueList& args );

		/** Ctor.
		 * Used by C++.
		 */
		Event();

		/** Register object.
		 * @param target Target variable.
		 */
		void register_object( Diluculum::LuaVariable target );

		/** Get number of system hooks.
		 * @return Number of system hooks.
		 */
		std::size_t get_num_system_hooks() const;

		/** Get number of class hooks.
		 * @return Number of class hooks.
		 */
		std::size_t get_num_class_hooks() const;

		/** Hook system event (Lua function).
		 * @param args event:number, callback:function
		 * @return nil
		 * @throws Diluculum::LuaError if invalid event or function given.
		 */
		Diluculum::LuaValueList hook_system_event( const Diluculum::LuaValueList& args );

		/** Hook class event (Lua function).
		 * @param args event:number, class:string, callback:function
		 * @return nil
		 * @throws Diluculum::LuaError if invalid event, class or function given.
		 */
		Diluculum::LuaValueList hook_class_event( const Diluculum::LuaValueList& args );

	private:
		typedef std::map<const int, Diluculum::LuaFunction> SystemFunctionMap;

		typedef std::pair<std::string, Diluculum::LuaFunction> ClassFunctionPair;
		typedef std::vector<ClassFunctionPair> ClassFunctionArray;
		typedef std::map<const int, ClassFunctionArray> ClassFunctionMap;

		SystemFunctionMap m_system_functions;
		ClassFunctionMap m_class_functions;
};

}
}
