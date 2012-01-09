#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Socket.hpp>
#include <FlexWorld/NonCopyable.hpp>

namespace flex {

/** The Peer class holds some basic information for server<->client connections.
 */
class Peer : public NonCopyable {
	public:
		Socket socket; ///< Socket.
		Protocol::Buffer buffer; ///< Buffer.
};

}
