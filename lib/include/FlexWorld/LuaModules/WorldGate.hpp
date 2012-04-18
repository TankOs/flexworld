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
};

}
}