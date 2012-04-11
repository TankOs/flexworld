#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <cstdint>

class ResourceManager;

namespace flex {
class Planet;
}

namespace sg {
class Transform;
}

/** Color picker for picking blocks and entities.
 *
 * The color picker uses color indexing. A ray is shot out along a forward
 * vector with a given distance. All blocks and entities being hit by the ray
 * are added to the scene's geometry and are color-coded. The picker renders
 * the scene in a very small viewport and picks the color of its center. If a
 * block was hit yet another rendering pass will happen that determines which
 * side of the block has been hit (each triangle is color-coded dependant on
 * the normal/facing). If an entity was hit only the proper entity ID is being
 * stored.
 */
class ColorPicker {
	public:
		/** Picking result.
		 */
		struct Result {
			typedef sf::Vector3<uint32_t> BlockPosition; ///< Block position type.

			/** Ctor.
			 */
			Result();

			/** Type of object that was hit.
			 */
			enum ObjectType {
				NONE = 0, ///< No object hit.
				BLOCK, ///< Block hit.
				ENTITY ///< Entity hit.
			};

			ObjectType m_type; ///< Type of hit object.
			BlockPosition m_block_position; ///< Position of hit block (in absolute planet coordinates).
			uint32_t m_entity_id; ///< ID of hit entity.
		};

		/** Pick block/entity.
		 * @param origin Origin vector.
		 * @param forward Forward vector.
		 * @param distance Distance scalar.
		 * @param transform Transform.
		 * @param pixel_pos Pixel position for picking.
		 * @param planet Planet with data of interest.
		 * @param resource_manager Resource manager for loading models.
		 * @return Result object with picking results.
		 */
		static Result pick(
			const sf::Vector3f& origin,
			const sf::Vector3f forward,
			float distance,
			const sg::Transform& transform,
			const sf::Vector2i& pixel_pos,
			const flex::Planet& planet,
			ResourceManager& resource_manager
		);

	private:
		ColorPicker();
		ColorPicker( const ColorPicker& other );
};
