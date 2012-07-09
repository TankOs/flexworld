#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Facing.hpp>

#include <SFML/System/Vector3.hpp>
#include <cstdint>

namespace fw {
namespace msg {

/** Block action network message.
 */
class BlockAction : public Message {
	public:
		typedef sf::Vector3<uint32_t> BlockPosition; ///< Block position type.

		/** Ctor.
		 */
		BlockAction();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get block position.
		 * @return Block position.
		 */
		const BlockPosition& get_block_position() const;

		/** Is action primary?
		 * @return true if primary.
		 */
		bool is_primary() const;

		/** Get facing.
		 * @return Facing.
		 */
		Facing get_facing() const;

		/** Set block position.
		 * @param position Block position.
		 */
		void set_block_position( const BlockPosition& position );

		/** Set primary.
		 * @param primary Primary.
		 */
		void set_primary( bool primary );

		/** Set facing.
		 * @param facing Facing.
		 */
		void set_facing( Facing facing );

	private:
		BlockPosition m_block_position;
		Facing m_facing;
		bool m_primary;
};

}
}
