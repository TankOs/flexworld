#pragma once

#include <FlexWorld/LuaModules/WorldGate.hpp>

#include <string>
#include <cstdint>

class ExampleWorldGate : public flex::lua::WorldGate {
	public:
		void destroy_block( const BlockPosition& block_position, const std::string& planet );
		void set_block( const BlockPosition& block_position, const std::string& planet, const flex::FlexID& cls );
};
