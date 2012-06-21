#pragma once

#include <SFML/System/String.hpp>
#include <Diluculum/LuaValue.hpp>
#include <Diluculum/LuaVariable.hpp>
#include <map>
#include <string>

namespace Diluculum {
	class LuaState;
}

namespace flex {

/** Lua modules.
 */
namespace lua {

/** Lua test module.
 * This module's main purpose is to test the scripting connectivity from unit
 * tests.
 */
class Test {
	public:
		/** Register class with state.
		 * @param variable Target variable.
		 */
		static void register_class( const Diluculum::LuaVariable& variable );

		/** Ctor.
		 * This ctor is called from Lua. It throws an error because constructing
		 * the module is prohibited there.
		 * @param args Arguments (ignored).
		 */
		Test( const Diluculum::LuaValueList& args );

		/** Ctor.
		 */
		Test();

		/** Register object with state.
		 * @param variable Target variable.
		 */
		void register_object( Diluculum::LuaVariable variable );

		/** Get number of values (Lua function).
		 * @param args Args (ignored).
		 * @return Number of values.
		 */
		Diluculum::LuaValueList get_num_values( const Diluculum::LuaValueList& args = Diluculum::LuaValueList() ) const;

		/** Set value.
		 * @param args Key:string, Value:string.
		 * @return nil.
		 */
		Diluculum::LuaValueList set_value( const Diluculum::LuaValueList& args );

		/** Find value.
		 * @param args Key:string.
		 * @return Value:string or nil if not set.
		 */
		Diluculum::LuaValueList find_value( const Diluculum::LuaValueList& args ) const;

	private:
		typedef std::map<const std::string, sf::String> StringMap;

		StringMap m_values;
};

}
}
