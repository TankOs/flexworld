#pragma once

#include <FlexWorld/Message.hpp>

#include <SFML/System/Vector3.hpp>
#include <string>
#include <cstdint>

namespace fw {
namespace msg {

/** Set block network message.
 */
class SetBlock : public Message {
	public:
		typedef sf::Vector3<uint32_t> BlockPosition; ///< Block position type.

		/** Ctor.
		 */
		SetBlock();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get block position.
		 * @return Block position.
		 */
		const BlockPosition& get_block_position() const;

		/** Get class ID.
		 * @return Class ID.
		 */
		const std::string& get_class_id() const;

		/** Set block position.
		 * @param block_position Block position.
		 */
		void set_block_position( const BlockPosition& block_position );

		/** Set class ID.
		 * @param class_id Class ID.
		 */
		void set_class_id( const std::string& class_id );

	private:
		BlockPosition m_block_position;
		std::string m_class_id;
};

}
}
