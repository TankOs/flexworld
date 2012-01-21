#include <FlexWorld/PackageEnumerator.hpp>

namespace fs = boost::filesystem;

namespace flex {

bool PackageEnumerator::enumerate( const std::string& path ) {
	m_class_files.clear();
	m_image_files.clear();
	m_model_files.clear();

	fs::path b_path( path );

	return enumerate( b_path );
}

bool PackageEnumerator::enumerate( const boost::filesystem::path& path ) {
	if( !fs::exists( path ) ) {
		return false;
	}

	fs::directory_iterator dir_iter( path );
	fs::directory_iterator dir_iter_end;

	for( ; dir_iter != dir_iter_end; ++dir_iter ) {
		// Recursively continue when we've got a directory.
		if( fs::is_directory( *dir_iter ) ) {
			if( !enumerate( *dir_iter ) ) {
				return false;
			}

			continue;
		}

		std::string filename = dir_iter->path().string();

		// Skip if extension is not long enough for recognized file types.
		if( filename.size() < 4 ) {
			continue;
		}

		// Check for class.
		if( filename.substr( filename.size() - 4 ) == ".yml" ) {
			m_class_files.push_back( filename );
		}
		else if( filename.substr( filename.size() - 4 ) == ".png" ) {
			m_image_files.push_back( filename );
		}
		else if( filename.substr( filename.size() - 4 ) == ".fwm" ) {
			m_model_files.push_back( filename );
		}
	}

	return true;
}

std::size_t PackageEnumerator::get_num_class_files() const {
	return m_class_files.size();
}

std::size_t PackageEnumerator::get_num_image_files() const {
	return m_image_files.size();
}

std::size_t PackageEnumerator::get_num_model_files() const {
	return m_model_files.size();
}

const std::string& PackageEnumerator::get_class_file( std::size_t index ) const {
	assert( index < m_class_files.size() );

	return m_class_files[index];
}

const std::string& PackageEnumerator::get_image_file( std::size_t index ) const {
	assert( index < m_image_files.size() );

	return m_image_files[index];
}

const std::string& PackageEnumerator::get_model_file( std::size_t index ) const {
	assert( index < m_model_files.size() );

	return m_model_files[index];
}

}
