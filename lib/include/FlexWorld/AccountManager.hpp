#pragma once

#include <FlexWorld/Account.hpp>

#include <map>

namespace fw {

/** Account manager.
 * The account manager is a simple storage class for managing accounts. Besides
 * of a normal container it doesn't allow to overwrite accounts. Instead,
 * accounts are updated.
 */
class AccountManager {
	public:
		/** Load strategy.
		 */
		enum LoadStrategy {
			CANCEL_ON_FAILURE = 0, ///< Stop loading on failure and restore previous state.
			CONTINUE_ON_FAILURE ///< Continue on failure and skip faulty account.
		};

		/** Ctor.
		 */
		AccountManager();

		/** Find existing account.
		 * @param username Username.
		 * @return Account or nullptr if not found.
		 */
		const Account* find_account( const std::string& username ) const;

		/** Add new account.
		 * @param account Account (username must be unique).
		 * @see find_account to check if an username is already taken.
		 */
		void add_account( const Account& account );

		/** Remove account.
		 * Undefined behaviour if no account with the specified username exists.
		 * @param username Username (must exist).
		 */
		void remove_account( const std::string& username );

		/** Get number of accounts.
		 * @return Number of accounts.
		 */
		std::size_t get_num_accounts() const;

		/** Update account.
		 * Undefined behaviour if username does not exist.
		 * @param account Account (account must be added before).
		 */
		void update_account( const Account& account );

		/** Load accounts from directory.
		 * After loading only those accounts from the directory will exist in the
		 * manager. Everything else is dropped.
		 * @param path_ Path.
		 * @param strategy Strategy for failures.
		 * @return Number of accounts loaded.
		 */
		std::size_t load_accounts_from_directory( const std::string& path_, LoadStrategy strategy );

	private:
		typedef std::map<const std::string, Account> AccountMap;

		AccountMap m_accounts;
};

}
