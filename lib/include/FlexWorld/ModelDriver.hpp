#pragma once

#include <FlexWorld/Exception.hpp>

#include <vector>
#include <string>

namespace fw {

class Model;

/** Driver for serializing and deserializing models.
 */
class ModelDriver {
	public:
		typedef std::vector<char> Buffer; ///< Buffer.

		/** Thrown when deserialization fails.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( DeserializationException );

		/** Serialize model.
		 * @param model Model.
		 * @return Serialized model.
		 */
		static Buffer serialize( const Model& model );

		/** Deserialize model.
		 * @param buffer Buffer.
		 * @return Model.
		 * @throws DeserializationException in case of invalid data.
		 */
		static Model deserialize( const Buffer& buffer );
};

}
