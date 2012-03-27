#include <FlexWorld/LuaModules/Event.hpp>
#include <FlexWorld/FlexID.hpp>

#include <Diluculum/LuaWrappers.hpp>
#include <iostream> // XXX TODO

namespace flex {
namespace lua {

DILUCULUM_BEGIN_CLASS( Event )
	DILUCULUM_CLASS_METHOD( Event, hook_system_event )
	DILUCULUM_CLASS_METHOD( Event, hook_class_event )
DILUCULUM_END_CLASS( Event )

void Event::register_class( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_CLASS( target, Event );

	// System events.
	target["System"] = Diluculum::EmptyTable;
	target["System"]["CONNECT"] = CONNECT_EVENT;
	target["System"]["DISCONNECT"] = DISCONNECT_EVENT;
	target["System"]["LOGIN"] = LOGIN_EVENT;
	target["System"]["LOGIN_FAIL"] = LOGIN_FAIL_EVENT;
	target["System"]["CHAT"] = CHAT_EVENT;
	target["System"]["UNLOAD"] = UNLOAD_EVENT;

	// Class events.
	target["Class"] = Diluculum::EmptyTable;
	target["Class"]["USE"] = USE_EVENT;
	target["Class"]["TAKE"] = TAKE_EVENT;
	target["Class"]["DROP"] = DROP_EVENT;
	target["Class"]["PRIMARY_ACTION"] = PRIMARY_ACTION_EVENT;
	target["Class"]["SECONDARY_ACTION"] = SECONDARY_ACTION_EVENT;
	target["Class"]["CREATE"] = CREATE_EVENT;
	target["Class"]["DESTROY"] = DESTROY_EVENT;
}

Event::Event( const Diluculum::LuaValueList& /*args*/ ) {
	throw Diluculum::LuaError( "Not allowed to instantiate Event." );
}

Event::Event() {
}

void Event::register_object( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_OBJECT( target, Event, *this );
}

std::size_t Event::get_num_system_hooks() const {
	return m_system_functions.size();
}

std::size_t Event::get_num_class_hooks() const {
	std::size_t num = 0;

	ClassFunctionMap::const_iterator cf_iter( m_class_functions.begin() );
	ClassFunctionMap::const_iterator cf_iter_end( m_class_functions.end() );
	
	for( ; cf_iter != cf_iter_end; ++cf_iter ) {
		num += cf_iter->second.size();
	}

	return num;
}

Diluculum::LuaValueList Event::hook_system_event( const Diluculum::LuaValueList& args ) {
	if( args.size() != 2 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for event." );
	}

	if( args[1].type() != LUA_TFUNCTION ) {
		throw Diluculum::LuaError( "Expected function for callback." );
	}

	int event_id = static_cast<int>( args[0].asInteger() );

	if( event_id < 0 || event_id >= NUM_SYSTEM_EVENTS ) {
		throw Diluculum::LuaError( "Invalid system event ID." );
	}

	m_system_functions[event_id] = args[1].asFunction();

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList Event::hook_class_event( const Diluculum::LuaValueList& args ) {
	if( args.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for event." );
	}

	if( args[1].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for class." );
	}

	if( args[2].type() != LUA_TFUNCTION ) {
		throw Diluculum::LuaError( "Expected function for callback." );
	}

	int event_id = static_cast<int>( args[0].asInteger() );
	std::string class_id = args[1].asString();

	if( event_id < 0 || event_id >= NUM_CLASS_EVENTS ) {
		throw Diluculum::LuaError( "Invalid class event ID." );
	}

	// Check ID.
	FlexID flex_id;

	if( !flex_id.parse( class_id ) || !flex_id.is_valid_resource() ) {
		throw Diluculum::LuaError( "Invalid class ID." );
	}

	std::cout << "TODO: Check that a class with the proper ID exists." << std::endl;

	m_class_functions[event_id].push_back( ClassFunctionPair( class_id, args[2].asFunction() ) );

	return Diluculum::LuaValueList();
}

}
}
