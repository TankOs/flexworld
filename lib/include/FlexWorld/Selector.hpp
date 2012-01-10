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

		/** Clear selector.
		 */
		void clear();

		/** Get number of added sockets.
		 * @return Number of added sockets.
		 */
		std::size_t get_num_sockets() const;

		/** Add socket.
		 * @param socket Socket (must be unique).
		 */
		void add( Socket& socket );

		/** Check if socket was added.
		 * @param socket Socket.
		 * @return true if added.
		 */
		bool has( Socket& socket ) const;

		/** Remove socket.
		 * @param socket Socket (must exist).
		 */
		void remove( Socket& socket );

		/** Select sockets.
		 * @param mode Mode.
		 * @param timeout Timeout, in µs (POLL for polling, BLOCK to block).
		 * @return Number of sockets ready for the proper operation.
		 * @see get_num_ready_sockets() to see how many sockets are ready for the proper operation.
		 * @see get_ready_socket() to access the ready sockets.
		 */
		std::size_t select( Mode mode, uint32_t timeout );

		/** Check if a socket is ready.
		 * @param socket Socket (must be added with add() before).
		 * @return true if ready.
		 */
		bool is_ready( const Socket& socket ) const;

	private:
		typedef std::vector<Socket*> SocketVector;

		fd_set m_fd_set;
		fd_set m_ready_set;
		SocketVector m_sockets;
		Socket::Handle m_max_socket;
};

}
