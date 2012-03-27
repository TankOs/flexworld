#include <FlexWorld/LuaModules/Test.hpp>

#include <Diluculum/LuaExceptions.hpp>
#include <Diluculum/LuaState.hpp>
#include <Diluculum/LuaWrappers.hpp>

namespace flex {
namespace lua {

DILUCULUM_BEGIN_CLASS( Test )
	DILUCULUM_CLASS_METHOD( Test, get_num_values )
	DILUCULUM_CLASS_METHOD( Test, set_value )
	DILUCULUM_CLASS_METHOD( Test, find_value )
DILUCULUM_END_CLASS( Test )

void Test::register_class( const Diluculum::LuaVariable& variable ) {
	DILUCULUM_REGISTER_CLASS( variable, Test );
}

Test::Test( const Diluculum::LuaValueList& /*args*/ ) {
	throw Diluculum::LuaError( "Not allowed to instanciate Test." );
}

Test::Test() {
}

void Test::register_object( Diluculum::LuaVariable variable ) {
	DILUCULUM_REGISTER_OBJECT( variable, Test, *this );
}

Diluculum::LuaValueList Test::get_num_values( const Diluculum::LuaValueList& /*args*/ ) const {
	Diluculum::LuaValueList ret;
	ret.push_back( static_cast<int>( m_values.size() ) );

	return ret;
}

Diluculum::LuaValueList Test::set_value( const Diluculum::LuaValueList& args ) {
	if( args.size() != 2 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for key." );
	}

	if( args[1].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for value." );
	}

	m_values[args[0].asString()] = args[1].asString();

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList Test::find_value( const Diluculum::LuaValueList& args ) const {
	if( args.size() != 1 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for key." );
	}

	StringMap::const_iterator value_iter = m_values.find( args[0].asString() );

	if( value_iter == m_values.end() ) {
		return Diluculum::LuaValueList();
	}

	Diluculum::LuaValueList ret;
	ret.push_back( value_iter->second );

	return ret;
}

}
}
