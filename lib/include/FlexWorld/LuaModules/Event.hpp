#pragma once

#include <Diluculum/LuaVariable.hpp>
#include <Diluculum/LuaFunction.hpp>

#include <SFML/System/String.hpp>
#include <map>
#include <vector>
#include <cstdint>

namespace Diluculum {
class LuaState;
}

namespace flex {

class Class;
class Entity;

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

		/** Check if a string is a valid command.
		 * @param command Command.
		 * @return true if valid.
		 */
		static bool is_valid_command( const std::string& command );

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

		/** Get number of command hooks.
		 * @return Number of command hooks.
		 */
		std::size_t get_num_command_hooks() const;

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

		/** Hook command (Lua function).
		 * @param args command:string, callback:function.
		 * @return nil
		 * @throws Diluculum::LuaError if invalid command or callback given.
		 */
		Diluculum::LuaValueList hook_command( const Diluculum::LuaValueList& args );

		/** Trigger connect system event.
		 * @param client_id Client ID.
		 * @param state Lua state to use for calling functions.
		 */
		void trigger_connect_system_event( uint16_t client_id, Diluculum::LuaState& state );

		/** Trigger chat system event.
		 * @param message Message.
		 * @param channel Channel.
		 * @param sender Sender.
		 * @param state Lua state.
		 */
		void trigger_chat_system_event( const sf::String& message, const sf::String& channel, uint16_t sender, Diluculum::LuaState& state );

		/** Trigger use class event.
		 * @param cls Class.
		 * @param entity Entity being used.
		 * @param actor Actor entity.
		 * @param state Lua state.
		 */
		void trigger_use_class_event( const Class& cls, const Entity& entity, const Entity& actor, Diluculum::LuaState& state );

		/** Trigger command.
		 * @param command Command.
		 * @param args Arguments.
		 * @param sender Sender client ID.
		 * @param state Lua state.
		 */
		void trigger_command( const std::string& command, const std::vector<sf::String>& args, uint16_t sender, Diluculum::LuaState& state );

	private:
		typedef std::vector<Diluculum::LuaFunction> FunctionArray;

		typedef std::vector<FunctionArray> SystemEventFunctionArray;

		typedef std::map<const std::string, FunctionArray> ClassFunctionsMap;
		typedef std::vector<ClassFunctionsMap> ClassEventFunctionsArray;

		typedef std::map<const std::string, Diluculum::LuaFunction> CommandFunctionMap;

		void call_system_event_callbacks( SystemEvent event, const Diluculum::LuaValueList& args, Diluculum::LuaState& state );
		void call_class_event_callbacks( ClassEvent event, const std::string& cls_id, const Diluculum::LuaValueList& args, Diluculum::LuaState& state );

		SystemEventFunctionArray m_system_functions;
		ClassEventFunctionsArray m_class_functions;
		CommandFunctionMap m_command_functions;
};

}
}
