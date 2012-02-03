#pragma once

#include <vector>
#include <string>

namespace flex {

class Model;

/** Driver for serializing and deserializing models.
 */
class ModelDriver {
	public:
		typedef std::vector<char> Buffer; ///< Buffer.

		/** Serialize model.
		 * @param model Model.
		 * @return Serialized model.
		 */
		static Buffer serialize( const Model& model );
};

}
