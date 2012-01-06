#pragma once

#include <FlexWorld/Protocol.hpp>

#include <cstdint>

namespace flex {

class Socket;

/** The Peer class holds some extra information for server->client connections.
 * Every peer is associated with a connection ID (reused!). Besides of that it
 * holds a reference to the socket and a buffer.
 */
class Peer {
	public:
		typedef uint32_t ID; ///< Connection ID type.

		/** Ctor.
		 */
		Peer();

		/** Ctor.
		 * @param id_ ID.
		 * @param socket_ Socket (pointer is stored).
		 */
		Peer( ID id_, Socket& socket_ );

		ID id; ///< Connection ID.
		Socket* socket; ///< Socket.
		Protocol::Buffer buffer; ///< Buffer.
};

}
