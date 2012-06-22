#pragma once

#include <SFML/System/Vector3.hpp>
#include <string>
#include <cstdint>

namespace flex {

class FlexID;

namespace lua {

/** Gate for World Lua module.
 */
class WorldGate {
	public:
		typedef sf::Vector3<uint32_t> BlockPosition; ///< Block position type.
		typedef sf::Vector3<float> EntityPosition; ///< Entity position type.

		/** Dtor.
		 */
		virtual ~WorldGate();

		/** Destroy block.
		 * @param block_position Block position.
		 * @param planet Planet ID.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual void destroy_block( const BlockPosition& block_position, const std::string& planet ) = 0;

		/** Set block.
		 * @param block_position Block position.
		 * @param planet Planet ID.
		 * @param cls Class ID.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual void set_block( const BlockPosition& block_position, const std::string& planet, const FlexID& cls ) = 0;

		/** Create entity at specific position.
		 * @param cls_id Class ID.
		 * @param position Block position.
		 * @param planet_id Planet ID.
		 * @return ID of created entity.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual uint32_t create_entity( const FlexID& cls_id, const EntityPosition& position, const std::string& planet_id ) = 0;

		/** Create entity attached to another entity.
		 * @param cls_id Class ID.
		 * @param parent_id Parent entity ID.
		 * @param hook_id Parent hook ID.
		 * @return ID of created entity.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual uint32_t create_entity( const FlexID& cls_id, uint32_t parent_id, const std::string& hook_id ) = 0;

		/** Create entity and stow it into container.
		 * @param cls_id Class ID.
		 * @param container_id Container entity ID.
		 * @return ID of created entity.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual uint32_t create_entity( const FlexID& cls_id, uint32_t container_id ) = 0;

		/** Get an entity's position.
		 * @param entity_id Entity ID.
		 * @param position Filled with entity's position in planet coordinates.
		 * @param planet_id Filled with the planet ID the entity is at.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual void get_entity_position( uint32_t entity_id, EntityPosition& position, std::string& planet_id ) = 0;
};

}
}
