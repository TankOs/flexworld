#pragma once

#include <string>

namespace flex {

class Account;

/** YAML account driver for loading and saving account objects.
 */
class AccountDriver {
	public:
		/** Serialize account.
		 * @param account Account.
		 * @return YAML or empty in case of error.
		 */
		static std::string serialize( const Account& account );

		/** Deserialize account.
		 * @param data Serialized data.
		 * @param account Deserialized account (untouched if deserializing fails).
		 * @return true on success.
		 */
		static bool deserialize( const std::string& data, Account& account );

	private:
};

}
