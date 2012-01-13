#pragma once

#include <FlexWorld/Message.hpp>

#include <string>

namespace flex {
namespace msg {

/** Login network message.
 */
class Login : public Message {
	public:
		static const uint8_t MAX_USERNAME_LENGTH = 24; ///< Maximum username length.

		/** Ctor.
		 */
		Login();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const Buffer& buffer );

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

	private:
		std::string m_username;
		std::string m_password;
};

}
}
