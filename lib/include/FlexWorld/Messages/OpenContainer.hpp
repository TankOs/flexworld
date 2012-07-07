#pragma once

#include <FlexWorld/Message.hpp>

#include <string>

namespace flex {
namespace msg {

/** OpenContainer network message.
 */
class OpenContainer : public Message {
	public:
		/** Ctor.
		 */
		OpenContainer();

		/** Set ID.
		 * @param id ID.
		 */
		void set_id( uint32_t id );

		/** Get ID.
		 * @return ID.
		 */
		uint32_t get_id() const;

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
		uint32_t m_id;
};

}
}
