#pragma once

#include <boost/filesystem.hpp>
#include <vector>
#include <string>

namespace flex {

/** PackageEnumerator searches for files of interests and stores their paths separately, depending on their type.
 */
class PackageEnumerator {
	public:
		/** Enumerate a directory.
		 * This method searches in the directories and all sub-directories for
		 * classes, images, models and scripts. All previous found resources are
		 * cleared, no matter if this method succeeds or not.
		 * @param path Base path.
		 * @return false if base path or any sub-directory isn't accessible.
		 */
		bool enumerate( const std::string& path );

		/** Get number of found class files.
		 * @return Number of class files.
		 */
		std::size_t get_num_class_files() const;

		/** Get number of image files.
		 * @return Number of image files.
		 */
		std::size_t get_num_image_files() const;

		/** Get number of model files.
		 * @return Number of model files.
		 */
		std::size_t get_num_model_files() const;

		/** Get number of script files.
		 * @return Number of script files.
		 */
		std::size_t get_num_script_files() const;

		/** Get class file.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Class file.
		 */
		const std::string& get_class_file( std::size_t index ) const;

		/** Get image file.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Image file.
		 */
		const std::string& get_image_file( std::size_t index ) const;

		/** Get model file.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Model file.
		 */
		const std::string& get_model_file( std::size_t index ) const;

		/** Get script file.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Script file.
		 */
		const std::string& get_script_file( std::size_t index ) const;

	private:
		typedef std::vector<std::string> StringVector;

		bool enumerate( const boost::filesystem::path& path );

		StringVector m_class_files;
		StringVector m_image_files;
		StringVector m_model_files;
		StringVector m_script_files;
};

}
