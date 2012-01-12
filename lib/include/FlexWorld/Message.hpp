#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Exception.hpp>

#include <cstdint>

namespace flex {

/** Baseclass for network messages.
 *
 * Properties should be checked when serializing/deserializing regarding the
 * message protocol definition.
 */
class Message {
	public:
		/** Thrown when serializing fails due to invalid properties.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( InvalidDataException );

		/** Thrown when deserializing fails due to bogus data.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( BogusDataException );

		/** Ctor.
		 */
		Message();

		/** Dtor.
		 */
		virtual ~Message();

		/** Serialize to buffer.
		 * Data is appended to the buffer.
		 * @param buffer Buffer.
		 * @throws InvalidDataException when any of the message's properties is invalid.
		 */
		virtual void serialize( Protocol::Buffer& buffer ) const = 0;

		/** Deserialize from buffer.
		 * Properties keep unchanged if deserialization fails.
		 * @param buffer Buffer.
		 * @return Number of bytes used for deserialization (0 means error).
		 */
		virtual std::size_t deserialize( const Protocol::Buffer& buffer ) = 0;

	private:
};

}
