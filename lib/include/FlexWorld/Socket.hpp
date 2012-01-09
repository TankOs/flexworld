#pragma once

#if defined( WINDOWS )
	// Only need the headers on windows.
	#include <FlexWorld/Networking.hpp>
#endif

#include <FlexWorld/NonCopyable.hpp>

#include <string>
#include <cstdint>

namespace flex {

class Selector;

/** Wrapper for cross-platform TCP socket usage.
 */
class Socket : public NonCopyable {
	public:
		friend class Selector;

		/** Ctor.
		 */
		Socket();

		/** Dtor.
		 */
		~Socket();

		/** Create socket.
		 */
		void create();

		/** Check if socket is connected.
		 * @return true if connected.
		 */
		bool is_connected() const;

		/** Check if socket is listening.
		 * @return true if listening.
		 */
		bool is_listening() const;

		/** Bind socket to address.
		 * @param ip IPv4 in dotted format.
		 * @param port Port.
		 * @return true on success.
		 */
		bool bind( const std::string& ip, uint16_t port );

		/** Go into listening state.
		 * @param backlog Backlog.
		 * @return true on success.
		 */
		bool listen( int backlog );

		/** Accept incoming connection.
		 * @param peer Socket that get initialized with incoming connection.
		 * @return true on success.
		 */
		bool accept( Socket& peer );

		/** Connect.
		 * @param ip Remote IP.
		 * @param port Remote port.
		 * @return true on success.
		 */
		bool connect( const std::string& ip, uint16_t port );

		/** Send data.
		 * @param buffer Buffer.
		 * @param size Buffer size.
		 * @return Amount of bytes sent, 0 is likely to be an error.
		 */
		std::size_t send( const char* buffer, std::size_t size );

		/** Receive data.
		 * @param buffer Buffer (gets filled with received data).
		 * @param size Buffer size (maximum bytes to receive).
		 * @return Amount of bytes received, 0 indicates error.
		 */
		std::size_t receive( char* buffer, std::size_t size );

		/** Close.
		 * This won't gracefully shutdown!
		 */
		void close();

		/** Shutdown connection.
		 */
		void shutdown();

	private:
		#if defined( LINUX )
			typedef int Handle; ///< Socket handle.
			static const Handle INVALID_SOCKET = -1; ///< Invalid socket handle.
		#elif defined( WINDOWS )
			typedef SOCKET Handle; ///< Socket handle.
			typedef int ssize_t; ///< Type for recv and send.
			static const int SHUT_WR = SD_SEND;
		#endif

		Handle m_socket;
		bool m_connected;
		bool m_listening;
};

}
