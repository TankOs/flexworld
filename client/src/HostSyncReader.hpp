#pragma once

#include <FWMS/Reader.hpp>

namespace fw {
class Client;
}

/** Host sync reader.
 *
 * The host synchronization reader listens for messages that need to send out
 * network messages to the host in order to synchonize data. Examples:
 *
 *   * Request chunks when view cuboid changes.
 *   * Request container contents when opening a container.
 *   * Send position updates when moving.
 */
class HostSyncReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		HostSyncReader();

		/** Set client.
		 * @param client Client.
		 */
		void set_client( fw::Client& client );

	private:
		void handle_message( const ms::Message& message );

		fw::Client* m_client;
};
