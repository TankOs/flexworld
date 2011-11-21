#pragma once

#include <string>

namespace flex {

class Class;

/** Class driver for loading and saving class files.
 */
class ClassDriver {
	public:
		/** Load from file.
		 * @param path Path.
		 * @param cls Loaded class (untouched in case of an error).
		 * @return true on success.
		 */
		static bool load( const std::string& path, Class& cls );

};

}
