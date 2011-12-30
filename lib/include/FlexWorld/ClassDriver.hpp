#pragma once

#include <FlexWorld/Class.hpp>
#include <FlexWorld/Exception.hpp>

#include <string>

namespace flex {

/** Class driver for loading and saving class files to and from YAML.
 */
class ClassDriver {
	public:
		/** Loader exception.
		 * Thrown when load() fails. Check what() for the reason.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( LoadException );

		/** Load from file.
		 * @param path Path.
		 * @return Loaded class.
		 * @throws LoadException if YAML parser failed to parse loaded data.
		 */
		static Class load( const std::string& path );
};

}
