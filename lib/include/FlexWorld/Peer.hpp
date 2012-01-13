#pragma once

#include <FlexWorld/Socket.hpp>
#include <FlexWorld/NonCopyable.hpp>
#include <FlexWorld/ServerProtocol.hpp>

#include <vector>

namespace flex {

/** The Peer class holds some basic information for server<->client connections.
 */
class Peer : public NonCopyable {
	public:
		Socket socket; ///< Socket.
		ServerProtocol::Buffer buffer; ///< Buffer.
};

}
