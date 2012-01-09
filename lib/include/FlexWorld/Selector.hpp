#pragma once

#include <FlexWorld/Socket.hpp>
#include <FlexWorld/Networking.hpp>

#include <vector>
#include <cstdint>

namespace flex {

/** Selector for select() calls.
 * Internally only references to the sockets are stored. Therefore make sure
 * to remove them as soon as they get invalid/closed! Everything else leads to
 * undefined behaviour.
 */
class Selector {
	public:
		/** Select mode.
		 */
		enum Mode {
			READ = 0,
			WRITE
		};

		static const uint32_t POLL; ///< Poll.
		static const uint32_t BLOCK; ///< Block.

		/** Ctor.
		 */
		Selector();

		/** Get number of added sockets.
		 * @return Number of added sockets.
		 */
		std::size_t get_num_sockets() const;

		/** Get number of ready sockets (i.e. the proper operation would not block).
		 * @return Number of ready sockets.
		 */
		std::size_t get_num_ready_sockets() const;

		/** Add socket.
		 * @param socket Socket (must be unique).
		 */
		void add_socket( Socket& socket );

		/** Check if socket was added.
		 * @param socket Socket.
		 * @return true if added.
		 */
		bool has_socket( Socket& socket ) const;

		/** Remove socket.
		 * @param socket Socket (must exist).
		 */
		void remove_socket( Socket& socket );

		/** Select sockets.
		 * @param mode Mode.
		 * @param timeout Timeout, in µs (POLL for polling, BLOCK to block).
		 * @see get_num_ready_sockets() to see how many sockets are ready for the proper operation.
		 * @see get_ready_socket() to access the ready sockets.
		 */
		void select( Mode mode, uint32_t timeout );

	private:
		typedef std::vector<Socket*> SocketVector;

		SocketVector m_sockets;
		SocketVector m_ready_sockets;

		fd_set m_fd_set;
		Socket::Handle m_max_socket;
};

}
