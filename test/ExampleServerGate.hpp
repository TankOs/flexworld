#pragma once

#include <FlexWorld/LuaModules/ServerGate.hpp>

#include <cstdint>

class ExampleServerGate : public flex::lua::ServerGate {
	public:
		const std::string& get_client_username( uint16_t client_id ) const;
};
