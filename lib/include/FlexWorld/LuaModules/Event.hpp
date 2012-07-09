#pragma once

#include <Diluculum/LuaVariable.hpp>
#include <Diluculum/LuaFunction.hpp>

#include <SFML/System/String.hpp>
#include <SFML/System/Vector3.hpp>
#include <map>
#include <vector>
#include <cstdint>

namespace Diluculum {
class LuaState;
}

namespace fw {

class Entity;

namespace lua {

/** Event Lua module.
 */
class Event {
	public:
		typedef sf::Vector3<uint32_t> BlockPosition; ///< Block position type.

		/** Event.
		 */
		enum {
			CONNECT_EVENT = 0, ///< Client connected.
			CHAT_EVENT, ///< Chat message.
			USE_EVENT, ///< Player used entity.
			BLOCK_ACTION_EVENT, ///< Player used primary action on block.
			NUM_EVENTS ///< Number of events.
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

		/** Get number of event hooks.
		 * @return Number of event hooks.
		 */
		std::size_t get_num_event_hooks() const;

		/** Get number of command hooks.
		 * @return Number of command hooks.
		 */
		std::size_t get_num_command_hooks() const;

		/** Hook event (Lua function).
		 * @param args event:number, callback:function
		 * @return nil
		 * @throws Diluculum::LuaError if invalid event or function given.
		 */
		Diluculum::LuaValueList hook_event( const Diluculum::LuaValueList& args );

		/** Hook command (Lua function).
		 * @param args command:string, callback:function.
		 * @return nil
		 * @throws Diluculum::LuaError if invalid command or callback given.
		 */
		Diluculum::LuaValueList hook_command( const Diluculum::LuaValueList& args );

		/** Trigger connect event.
		 * @param client_id Client ID.
		 * @param state Lua state to use for calling functions.
		 */
		void trigger_connect_event( uint16_t client_id, Diluculum::LuaState& state );

		/** Trigger chat event.
		 * @param message Message.
		 * @param channel Channel.
		 * @param sender Sender.
		 * @param state Lua state.
		 */
		void trigger_chat_event( const sf::String& message, const sf::String& channel, uint16_t sender, Diluculum::LuaState& state );

		/** Trigger use event.
		 * @param entity Entity being used.
		 * @param actor Actor entity.
		 * @param client_id Client ID.
		 * @param state Lua state.
		 */
		void trigger_use_event( const Entity& entity, const Entity& actor, uint16_t client_id, Diluculum::LuaState& state );

		/** Trigger block action event.
		 * @param block_pos Block position of block the action is performed on.
		 * @param next_block_pos Block position of block's neighbour (depending on which face the player clicked on).
		 * @param primary true if primary action, otherwise secondary.
		 * @param actor Actor entity (either entity in hand or player entity itself if not holding anything).
		 * @param client_id Client ID.
		 * @param state Lua state.
		 */
		void trigger_block_action_event(
			const BlockPosition& block_pos,
			const BlockPosition& next_block_pos,
			bool primary,
			const fw::Entity& actor,
			uint16_t client_id,
			Diluculum::LuaState& state
		);

		/** Trigger command.
		 * @param command Command.
		 * @param args Arguments.
		 * @param sender Sender client ID.
		 * @param state Lua state.
		 */
		void trigger_command( const std::string& command, const std::vector<sf::String>& args, uint16_t sender, Diluculum::LuaState& state );

	private:
		typedef std::vector<Diluculum::LuaFunction> FunctionArray;

		typedef std::vector<FunctionArray> EventFunctionArray;
		typedef std::map<const std::string, Diluculum::LuaFunction> CommandFunctionMap;

		void call_event_callbacks( int event, const Diluculum::LuaValueList& args, Diluculum::LuaState& state );

		EventFunctionArray m_event_functions;
		CommandFunctionMap m_command_functions;
};

}
}
