#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Planet.hpp>

#include <string>

namespace flex {
namespace msg {

/** ChunkUnchanged network message.
 */
class ChunkUnchanged : public Message {
	public:
		/** Ctor.
		 */
		ChunkUnchanged();

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const Planet::Vector& pos );

		/** Get position.
		 * @return Position.
		 */
		const Planet::Vector& get_position() const;

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
		Planet::Vector m_position;
};

}
}
