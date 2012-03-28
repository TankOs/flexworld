#include <FlexWorld/PackageEnumerator.hpp>

namespace fs = boost::filesystem;

namespace flex {

bool PackageEnumerator::enumerate( const std::string& path ) {
	m_script_files.clear();

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

		if( filename.substr( filename.size() - 4 ) == ".lua" ) {
			m_script_files.push_back( filename );
		}
	}

	return true;
}

std::size_t PackageEnumerator::get_num_script_files() const {
	return m_script_files.size();
}

const std::string& PackageEnumerator::get_script_file( std::size_t index ) const {
	assert( index < m_script_files.size() );

	return m_script_files[index];
}

}
