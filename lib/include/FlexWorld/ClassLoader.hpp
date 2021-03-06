#pragma once

#include <FlexWorld/Class.hpp>
#include <FlexWorld/Exception.hpp>

#include <string>

namespace fw {

class FlexID;

/** Loader for loading classes from multiple path sources.
 *
 * Take care of when you add what search path. Search paths added earlier get
 * higher priority!
 */
class ClassLoader {
	public:
		/** Thrown when loading a class fails.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( LoadException );

		/** Ctor.
		 */
		ClassLoader();

		/** Add search path.
		 * If the same path has been added already the operation is cancelled. A
		 * trailing slash is appended if not already present.
		 * @param path Path.
		 */
		void add_search_path( const std::string& path );

		/** Get number of search paths.
		 * @return Number of search paths.
		 */
		std::size_t get_num_search_paths() const;

		/** Get nth search path.
		 * @param index Index (must be valid).
		 * @return Search path.
		 */
		const std::string& get_search_path( std::size_t index ) const;

		/** Load class.
		 * Not only the class is loaded, it'll also be checked if the referenced
		 * assets are valid, i.e. exist in the filesystem.
		 * Make sure to add search paths before.
		 * @param id Class ID (must be a valid resource).
		 * @return Loaded class.
		 * @throw LoadException if class can't be loaded.
		 */
		Class load( const FlexID& id ) const;

	private:
		typedef std::vector<std::string> StringArray;

		std::string find_file( const std::string& filename ) const;

		StringArray m_search_paths;
};

}
