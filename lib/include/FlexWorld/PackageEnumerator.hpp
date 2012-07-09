#pragma once

#include <boost/filesystem.hpp>
#include <vector>
#include <string>

namespace fw {

/** PackageEnumerator searches for scripts in packages.
 */
class PackageEnumerator {
	public:
		/** Enumerate a directory.
		 * This method searches in the directories and all sub-directories for
		 * scripts. All previous found resources are cleared, no matter if this
		 * method succeeds or not.
		 * @param path Base path.
		 * @return false if base path or any sub-directory isn't accessible.
		 */
		bool enumerate( const std::string& path );

		/** Get number of script files.
		 * @return Number of script files.
		 */
		std::size_t get_num_script_files() const;

		/** Get script file.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Script file.
		 */
		const std::string& get_script_file( std::size_t index ) const;

	private:
		typedef std::vector<std::string> StringVector;

		bool enumerate( const boost::filesystem::path& path );

		StringVector m_script_files;
};

}
