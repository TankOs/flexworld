#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Planet.hpp>

#include <string>

namespace flex {
namespace msg {

/** CreateEntity network message.
 */
class CreateEntity : public Message {
	public:
		typedef uint32_t Timestamp; ///< Timestamp.

		/** Ctor.
		 */
		CreateEntity();

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const Planet::Coordinate& pos );

		/** Get position.
		 * @return Position.
		 */
		const Planet::Coordinate& get_position() const;

		/** Set heading.
		 * @param heading Heading.
		 */
		void set_heading( uint8_t heading );

		/** Get heading.
		 * @return Heading.
		 */
		uint8_t get_heading() const;

		/** Set class.
		 * @param cls Class.
		 */
		void set_class( const std::string& cls );

		/** Get class.
		 * @return Class.
		 */
		const std::string& get_class() const;

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
		std::string m_class;
		Planet::Coordinate m_position;
		uint8_t m_heading;
};

}
}
