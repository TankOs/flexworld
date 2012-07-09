#pragma once

#include <FlexWorld/Class.hpp>
#include <FlexWorld/Exception.hpp>

#include <string>

namespace fw {

/** Class driver for loading and saving class files to and from YAML.
 */
class ClassDriver {
	public:
		/** Loader exception.
		 * Thrown when load() fails. Check what() for the reason.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( LoadException );

		/** Load from file.
		 * If loading fails the cls parameter keeps untouched.
		 * @param path Path.
		 * @param cls Class in which loaded data is stored into.
		 * @throws LoadException if YAML parser failed to parse loaded data.
		 */
		static void load( const std::string& path, Class& cls );
};

}
