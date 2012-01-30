#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Planet.hpp>

#include <string>

namespace flex {
namespace msg {

/** RequestChunk network message.
 */
class RequestChunk : public Message {
	public:
		typedef uint32_t Timestamp; ///< Timestamp.

		/** Ctor.
		 */
		RequestChunk();

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const Planet::Vector& pos );

		/** Get position.
		 * @return Position.
		 */
		const Planet::Vector& get_position() const;

		/** Set timestamp.
		 * @param timestamp Timestamp.
		 */
		void set_timestamp( Timestamp timestamp );

		/** Get timestamp.
		 * @return Timestamp.
		 */
		Timestamp get_timestamp() const;

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
		Planet::Vector m_position;
		Timestamp m_timestamp;
};

}
}
