#pragma once

#include <string>
#include <cstdint>

namespace flex {
namespace lua {

/** Gate class for the server Lua module, abstract.
 */
class ServerGate {
	public:
		/** Dtor.
		 */
		virtual ~ServerGate();

		/** Get username of connected client.
		 * @param client_id Client ID.
		 * @return Username.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual const std::string& get_client_username( uint16_t client_id ) const = 0;

		/** Get number of connected clients.
		 * @return Number of connected clients.
		 * @throws std::runtime_error in case of any error.
		 */
		virtual std::size_t get_num_connected_clients() const = 0;

	private:
};

}
}
