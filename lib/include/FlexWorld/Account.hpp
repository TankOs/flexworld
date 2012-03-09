#pragma once

#include <FlexWorld/Entity.hpp>

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

		/** Set entity ID.
		 * @param id ID.
		 */
		void set_entity_id( Entity::ID id );

		/** Get username.
		 * @return Username.
		 */
		const std::string& get_username() const;

		/** Get password.
		 * @return Password.
		 */
		const std::string& get_password() const;

		/** Get entity ID.
		 * @return ID.
		 */
		Entity::ID get_entity_id() const;

	private:
		std::string m_username;
		std::string m_password;

		Entity::ID m_entity_id;
};

}
