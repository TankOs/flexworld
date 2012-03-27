#include "LuaUtils.hpp"

#include <Diluculum/LuaState.hpp>
#include <Diluculum/LuaExceptions.hpp>
#include <iostream>

bool check_error( const std::string& error, const std::string& code, Diluculum::LuaState& state ) {
	try {
		state.doString( code );

		return false;
	}
	catch( const Diluculum::LuaError& e ) {
		std::string str = e.what();
		std::string message = str.substr( str.find( ":" ) + 2 );

		if( error != message ) {
			std::cout
				<< "*** Expected: " << error << std::endl
				<< "*** Got:      " << message << std::endl
			;
			return false;
		}

		return true;
	}
	
	return false;
}


