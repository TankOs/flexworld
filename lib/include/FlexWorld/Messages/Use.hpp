#pragma once

#include <FlexWorld/Message.hpp>

namespace fw {
namespace msg {

/** Use message.
 */
class Use : public Message {
	public:
		/** Ctor.
		 */
		Use();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get entity ID.
		 * @return Entity ID.
		 */
		uint32_t get_entity_id() const;

		/** Set entity ID.
		 * @param id ID.
		 */
		void set_entity_id( uint32_t id );

	private:
		uint32_t m_entity_id;
};

}
}
