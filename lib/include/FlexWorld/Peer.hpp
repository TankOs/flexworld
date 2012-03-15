#pragma once

#include <FlexWorld/NonCopyable.hpp>
#include <FlexWorld/ServerProtocol.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include <memory>

namespace flex {

/** The Peer class holds some basic information for server<->client connections.
 */
class Peer : public NonCopyable {
	public:
		// XXX
		Peer();

		enum {
			READ_BUFFER_SIZE = 1024
		};

		typedef ServerProtocol::ConnectionID ConnectionID; ///< Connection ID.

		char read_buffer[READ_BUFFER_SIZE]; ///< Temporary read buffer.
		ServerProtocol::Buffer buffer; ///< Buffer.
		std::string ip; ///< IP.
		ConnectionID id; ///< Connection ID.
		std::unique_ptr<boost::asio::ip::tcp::socket> socket; ///< Socket.
};

}
