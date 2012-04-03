#pragma once

#include <FlexWorld/Message.hpp>

#include <SFML/System/Vector3.hpp>
#include <cstdint>

namespace flex {
namespace msg {

/** Destroy block network message.
 */
class DestroyBlock : public Message {
	public:
		typedef sf::Vector3<uint32_t> BlockPosition; ///< Block position type.

		/** Ctor.
		 */
		DestroyBlock();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get block position.
		 * @return Block position.
		 */
		const BlockPosition& get_block_position() const;

		/** Set block position.
		 * @param position Block position.
		 */
		void set_block_position( const BlockPosition& position );

	private:
		BlockPosition m_block_position;
};

}
}
