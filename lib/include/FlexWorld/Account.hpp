#pragma once

#include <string>

namespace flex {

/** Account.
 */
class Account {
	public:
		/** Ctor.
		 */
		Account();

		/** Set username.
		 * @param username Username.
		 */
		void set_username( const std::string& username );

		/** Set password.
		 * @param password Password.
		 */
		void set_password( const std::string& password );

		/** Get username.
		 * @return Username.
		 */
		const std::string& get_username() const;

		/** Get password.
		 * @return Password.
		 */
		const std::string& get_password() const;

	private:
		std::string m_username;
		std::string m_password;
};

}
