#pragma once

#include <FlexWorld/Socket.hpp>

#include <string>
#include <memory>
#include <cstdint>

namespace flex {

class Protocol;

/** Server for handling peers and traffic.
 */
class Server {
	public:
		/** Ctor.
		 * @param protocol Protocol.
		 */
		Server( Protocol& protocol );

		/** Dtor.
		 * Make sure to shut the server down gracefully with shutdown() before.
		 * Read the notes there!
		 */
		~Server();

		/** Start listening.
		 * Start the TCP listener. This will return immediately without processing
		 * further. Use process() to accept connections and process incoming and
		 * outgoing data.
		 * @param ip IP to listen on.
		 * @param port Port to listen on.
		 * @return false if failed to listen.
		 */
		bool listen( const std::string& ip, uint16_t port );

		/** Process incoming connections, receive and send data in buffers.
		 * This function will not block!
		 * @return Number of operations processed (1 action = receive or accept).
		 * @see listen() Must be called before.
		 */
		std::size_t process();

		/** Check if server is listening.
		 * @return true if listening.
		 */
		bool is_listening() const;

		/** Get number of connected clients.
		 * @return Number of connected clients.
		 */
		std::size_t get_num_clients() const;

		/** Shutdown server.
		 * Gracefully shutdown all connections. You should wait until
		 * is_listening() and get_num_clients() are both zero, otherwise
		 * connections may go into TIME_WAIT.
		 */
		void shutdown();

	private:
		static const std::size_t SET_SIZE = FD_SETSIZE;

		std::unique_ptr<Socket> m_listener;
		Protocol& m_protocol;
		fd_set m_fd_set;
		bool m_listening;
};

}
