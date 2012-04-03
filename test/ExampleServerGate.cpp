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

std::size_t ExampleServerGate::get_num_connected_clients() const {
	return 123;
}

void ExampleServerGate::broadcast_chat_message( const sf::String& message, const sf::String& channel, const sf::String& sender ) {
	if( message != "Message" ) {
		throw std::runtime_error( "Invalid message." );
	}

	if( channel != "Channel" ) {
		throw std::runtime_error( "Invalid channel." );
	}

	if( sender != "Sender" ) {
		throw std::runtime_error( "Invalid sender." );
	}
}
