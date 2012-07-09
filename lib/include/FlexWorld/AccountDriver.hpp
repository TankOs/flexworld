#pragma once

#include <FlexWorld/Exception.hpp>

#include <string>

namespace fw {

class Account;

/** YAML account driver for loading and saving account objects.
 */
class AccountDriver {
	public:
		/** Deserialize exception.
		 * Thrown when deserialize() fails. Check what() for the reason.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( DeserializeException );

		/** Serialize account.
		 * @param account Account.
		 * @return YAML (output).
		 */
		static std::string serialize( const Account& account );

		/** Deserialize account.
		 * @param data Serialized data.
		 * @return Deserialized account.
		 * @throws DeserializeException when parsing the data failed.
		 */
		static Account deserialize( const std::string& data );

	private:
};

}
