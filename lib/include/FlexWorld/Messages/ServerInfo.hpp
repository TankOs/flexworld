#pragma once

#include <FlexWorld/Message.hpp>

#include <string>

namespace flex {
namespace msg {

/** ServerInfo network message.
 */
class ServerInfo : public Message {
	public:
		/** Authentication mode.
		 */
		enum AuthMode {
			OPEN_AUTH = 0,
			KEY_AUTH
		};

		/** Flags.
		 */
		enum Flags {
			NO_FLAGS = 0,
			PASSWORD_FLAG = 1 << 0
		};

		/** Ctor.
		 */
		ServerInfo();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Set auth mode.
		 * @param mode Auth mode.
		 */
		void set_auth_mode( AuthMode mode );

		/** Get auth mode.
		 * @return Auth mode.
		 */
		AuthMode get_auth_mode() const;

		/** Set flags.
		 * @param flags Flags.
		 */
		void set_flags( uint8_t flags );

		/** Get flags.
		 * @return Flags.
		 */
		uint8_t get_flags() const;

	private:
		uint8_t m_auth_mode;
		uint8_t m_flags;
};

}
}
