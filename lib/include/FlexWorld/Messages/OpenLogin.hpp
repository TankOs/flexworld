#pragma once

#include <FlexWorld/Message.hpp>

#include <string>

namespace flex {
namespace msg {

/** OpenLogin network message.
 */
class OpenLogin : public Message {
	public:
		static const uint8_t MAX_USERNAME_LENGTH = 24; ///< Maximum username length.

		/** Ctor.
		 */
		OpenLogin();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Set username.
		 * @param username Username,
		 */
		void set_username( const std::string& username );

		/** Get username.
		 * @return Username.
		 */
		const std::string& get_username() const;

		/** Set password.
		 * @param password Password.
		 */
		void set_password( const std::string& password );

		/** Get password.
		 * @return Password.
		 */
		const std::string& get_password() const;

		/** Set server password.
		 * @param server_password Server password.
		 */
		void set_server_password( const std::string& server_password );

		/** Get server password.
		 * @return Server password.
		 */
		const std::string& get_server_password() const;

	private:
		std::string m_username;
		std::string m_password;
		std::string m_server_password;
};

}
}
