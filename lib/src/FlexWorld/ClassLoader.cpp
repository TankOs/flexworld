#include <FlexWorld/ClassLoader.hpp>
#include <FlexWorld/ClassDriver.hpp>

#include <boost/filesystem.hpp>
#include <algorithm>
#include <cassert>

namespace flex {

ClassLoader::ClassLoader() {
}

void ClassLoader::add_search_path( const std::string& path ) {
	assert( path.size() > 0 );

	// Skip if already added.
	StringArray::iterator path_iter = std::find( m_search_paths.begin(), m_search_paths.end(), path );

	if( path_iter != m_search_paths.end() ) {
		return;
	}

	// Append trailing slash.
	std::string final_path = path;

	if( final_path[final_path.size() - 1] != '/' ) {
		final_path += '/';
	}

	m_search_paths.push_back( final_path );
}

std::size_t ClassLoader::get_num_search_paths() const {
	return m_search_paths.size();
}

const std::string& ClassLoader::get_search_path( std::size_t index ) const {
	assert( index < m_search_paths.size() );

	return m_search_paths[index];
}

Class ClassLoader::load( const FlexID& id ) const {
	assert( id.is_valid_resource() );
	assert( m_search_paths.size() > 0 );

	// Build filename.
	std::string class_path = id.as_path() + ".yml";
	std::string found_path = "";

	// Search file.
	for( std::size_t path_idx = 0; path_idx < m_search_paths.size(); ++path_idx ) {
		std::string full_path = m_search_paths[path_idx] + class_path;

		// If it exists then remember the path and stop searching.
		if( boost::filesystem::exists( full_path ) ) {
			found_path = full_path;
			break;
		}
	}

	// If not found throw.
	if( found_path.empty() ) {
		throw LoadException( "File not found." );
	}

	try {
		// Load class.
		Class cls = ClassDriver::load( found_path );

		// Loading succeeded, return the class.
		return cls;
	}
	catch( const ClassDriver::LoadException& e ) {
		// Failed to load, rethrow.
		throw LoadException( "Failed to load." );
	}

	// Will never reach this point!
	assert( 0 );
	return Class( FlexID::make( "zomg/wtf" ) );
}

}
