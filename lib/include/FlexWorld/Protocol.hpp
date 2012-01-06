#pragma once

#include <FlexWorld/Exception.hpp>

#include <vector>

namespace flex {

class Socket;

/** Abstract class for protocols.
 */
class Protocol {
	public:
		typedef std::vector<char> Buffer; ///< Buffer.

		/** Thrown when handle_incoming_data() detected a bogus message.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( BogusMessageException );

		/** Dtor.
		 */
		virtual ~Protocol();

		/** Handle connect.
		 * @param socket Socket that connected.
		 */
		virtual void handle_connect( Socket& socket );

		/** Handle disconnect.
		 * @param socket Socket that disconnected.
		 */
		virtual void handle_disconnect( Socket& socket );

		/** Handle incoming data.
		 * This function doesn't change the buffer but instead reports how many
		 * data has been processed successfully and can (should!) be removed from
		 * the buffer (from the beginning).
		 * @param socket Socket that sent data.
		 * @param buffer Data.
		 * @return Number of bytes processed.
		 * @throws BogusMessageException in case this function detects a bad message.
		 */
		virtual std::size_t handle_incoming_data( Socket& socket, const Buffer& buffer );

	private:
};

}
