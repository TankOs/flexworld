#pragma once

#include <FlexWorld/Exception.hpp>

#include <vector>

namespace flex {

class Socket;

/** Abstract class for protocols.
 */
class Protocol {
	public:
		typedef std::size_t ConnectionID; ///< Connection ID.
		typedef std::vector<char> Buffer; ///< Buffer.

		/** Thrown when handle_incoming_data() detected a bogus message.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( BogusMessageException );

		/** Dtor.
		 */
		virtual ~Protocol();

		/** Handle connect.
		 * @param id Connection ID.
		 */
		virtual void handle_connect( ConnectionID id );

		/** Handle disconnect.
		 * @param id Connection ID.
		 */
		virtual void handle_disconnect( ConnectionID id );

		/** Handle incoming data.
		 * This function doesn't change the buffer but instead reports how many
		 * data has been processed successfully and can (should!) be removed from
		 * the buffer (from the beginning).
		 * @param id Connection ID.
		 * @param buffer Data.
		 * @return Number of bytes processed.
		 * @throws BogusMessageException in case this function detects a bad message.
		 */
		virtual std::size_t handle_incoming_data( ConnectionID id, const Buffer& buffer );

	private:
};

}
