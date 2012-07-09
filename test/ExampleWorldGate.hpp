#pragma once

#include <FlexWorld/LuaModules/WorldGate.hpp>

#include <string>
#include <cstdint>

class ExampleWorldGate : public fw::lua::WorldGate {
	public:
		void destroy_block( const BlockPosition& block_position, const std::string& planet );
		void set_block( const BlockPosition& block_position, const std::string& planet, const fw::FlexID& cls );
		uint32_t create_entity( const fw::FlexID& cls_id, const EntityPosition& position, const std::string& planet_id );
		uint32_t create_entity( const fw::FlexID& cls_id, uint32_t parent_id, const std::string& hook_id );
		uint32_t create_entity( const fw::FlexID& cls_id, uint32_t container_id );
		void get_entity_position( uint32_t entity_id, EntityPosition& position, std::string& planet_id );
		std::string get_entity_class_id( uint32_t entity_id );
};
