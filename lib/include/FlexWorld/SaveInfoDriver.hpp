#pragma once

#include <FlexWorld/SaveInfo.hpp>
#include <FlexWorld/Exception.hpp>

#include <string>

namespace flex {

/** Driver for loading SaveInfo objects from and to YAML.
 */
class SaveInfoDriver {
	public:
		/** Thrown when deserializing fails.
		 * Check what() for detailed info.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( DeserializeException );

		/** Deserialize SaveInfo.
		 * @param buffer Buffer with serialized data.
		 * @return Deserialized SaveInfo.
		 * @throws DeserializeException when parsing fails.
		 */
		static SaveInfo deserialize( const std::string& buffer );

		/** Serialize SaveInfo.
		 * @param info SaveInfo.
		 * @return Serialized data.
		 */
		static std::string serialize( const SaveInfo& info );

	private:
};

}
