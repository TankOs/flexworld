#include "ExampleServerGate.hpp"

#include <string>
#include <stdexcept>

const std::string& ExampleServerGate::get_client_username( uint16_t client_id ) const {
	if( client_id != 4711 ) {
		throw std::runtime_error( "Invalid client ID." );
	}

	static const std::string h4x0r = "H4x0r";
	return h4x0r;
}

