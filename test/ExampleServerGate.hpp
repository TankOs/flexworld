#pragma once

#include <FlexWorld/LuaModules/ServerGate.hpp>

#include <cstdint>

class ExampleServerGate : public flex::lua::ServerGate {
	public:
		const std::string& get_client_username( uint16_t client_id ) const;
		std::size_t get_num_connected_clients() const;
		void broadcast_chat_message( const sf::String& message, const sf::String& channel, const sf::String& sender );
		uint32_t get_client_entity_id( uint32_t client_id ) const;
};
