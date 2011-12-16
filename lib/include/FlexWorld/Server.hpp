#pragma once

#include <SFML/Network/TcpListener.hpp>
#include <string>
#include <cstdint>

namespace flex {

/** FlexWorld server instance.
 */
class Server {
	public:
		/** Authentication modes.
		 */
		enum AuthMode {
			OPEN_MODE = 0, ///< Requires username & password.
			ANNOUNCE_MODE ///< Does handshake with FlexWorld account authority.
		};

		/** Ctor.
		 */
		Server();

		/** Set authentication mode.
		 * @param mode Mode.
		 */
		void set_auth_mode( AuthMode mode );

		/** Start listening.
		 * @param ip IP (empty to listen on all interfaces).
		 * @param port Port.
		 * @return false when failed to listen.
		 */
		bool listen( const std::string& ip, uint16_t port = 2593 );

		/** Shutdown server gracefully.
		 */
		void shutdown();

	private:
		bool m_run;
		AuthMode m_auth_mode;

		sf::TcpListener m_tcp_listener;
};

}
