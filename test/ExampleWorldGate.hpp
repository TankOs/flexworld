#pragma once

#include <FlexWorld/LuaModules/WorldGate.hpp>

#include <string>
#include <cstdint>

class ExampleWorldGate : public flex::lua::WorldGate {
	public:
		void destroy_block( const BlockPosition& block_position, const std::string& planet );
		void set_block( const BlockPosition& block_position, const std::string& planet, const flex::FlexID& cls );
		void create_entity( const flex::FlexID& cls_id, const EntityPosition& position, const std::string& planet_id );
		void create_entity( const flex::FlexID& cls_id, uint32_t parent_id, const std::string& hook_id );
		void create_entity( const flex::FlexID& cls_id, uint32_t container_id );
		void get_entity_position( uint32_t entity_id, EntityPosition& position, std::string& planet_id );
};
