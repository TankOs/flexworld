#pragma once

#include <Diluculum/LuaVariable.hpp>

namespace flex {
namespace lua {

class WorldGate;

/** World Lua module.
 */
class World {
	public:
		/** Register class with Lua state.
		 * @param target Target variable.
		 */
		static void register_class( Diluculum::LuaVariable target );

		/** Ctor called by Lua (forbidden).
		 * @param args Arguments.
		 */
		World( const Diluculum::LuaValueList& args );

		/** Ctor for C++.
		 * @param gate Gate.
		 */
		World( WorldGate& gate );

		/** Register object with Lua state.
		 * @param target Target variable.
		 */
		void register_object( Diluculum::LuaVariable target );

		/** Destroy block (Lua function).
		 * @param args block_position:table(x,y,z) planet:string
		 * @return nil
		 */
		Diluculum::LuaValueList destroy_block( const Diluculum::LuaValueList& args );

		/** Set block (Lua function).
		 * @param args block_position:table(x,y,z) planet:string class:string
		 * @return nil
		 */
		Diluculum::LuaValueList set_block( const Diluculum::LuaValueList& args );

		/** Create entity (Lua function).
		 * @param args class:string block_position:table(x,y,z) planet:string
		 * @return nil
		 */
		Diluculum::LuaValueList create_entity( const Diluculum::LuaValueList& args );

	private:
		WorldGate* m_gate;
};

}
}
