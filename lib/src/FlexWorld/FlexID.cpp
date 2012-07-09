#include <FlexWorld/FlexID.hpp>

#include <cassert>

namespace fw {

FlexID FlexID::make( const std::string& string ) {
	FlexID id;
	if( !id.parse( string ) ) {
		throw ParserException( "Failed to parse ID." );
	}

	return id;
}

FlexID::FlexID() {
}

bool FlexID::parse( const std::string& id ) {
	if( id.empty() ) {
		return false;
	}

	// Working ID.
	FlexID new_id;
	std::string package;
	std::string resource;

	// Find package-resource-delimiter.
	std::size_t delim_pos( id.find( "/" ) );

	// Delimiter shall not be at last position.
	if( delim_pos + 1 == id.size() ) {
		return false;
	}

	// If not found, use package only.
	if( delim_pos == std::string::npos ) {
		package = id;
	}
	else {
		package = id.substr( 0, delim_pos );
		resource = id.substr( delim_pos + 1 );
	}

	// Set package/resource and check results.
	if( !new_id.set_package( package ) ) {
		return false;
	}

	if( resource.size() && !new_id.set_resource( resource ) ) {
		return false;
	}

	// Success! Apply new ID.
	std::swap( *this, new_id );

	return true;
}

const std::string& FlexID::get_package() const {
	return m_package;
}

const std::string& FlexID::get_resource() const {
	return m_resource;
}

std::string FlexID::get() const {
	if( !is_valid_package() ) {
		return "";
	}
	else if( !is_valid_resource() ) {
		return m_package;
	}

	return m_package + "/" + m_resource;
}

bool FlexID::operator==( const FlexID& other ) const {
	return m_package == other.m_package && m_resource == other.m_resource;
}

bool FlexID::operator!=( const FlexID& other ) const {
	return m_package != other.m_package || m_resource != other.m_resource;
}

bool FlexID::is_valid_package() const {
	return !m_package.empty();
}

bool FlexID::is_valid_resource() const {
	return is_valid_package() && !m_resource.empty();
}

bool FlexID::set_package( const std::string& package ) {
	// Catch simple mistakes.
	if( package.empty() || package[0] == '.' || package[package.size() - 1] == '.' ) {
		return false;
	}

	for( std::size_t ch_index = 0; ch_index < package.size(); ++ch_index ) {
		char ch( package[ch_index] );

		if( ch == '.' ) {
			// No multiple dots.
			if( ch_index > 0 && package[ch_index - 1] == '.' ) {
				return false;
			}
		}
		else if(
			(ch < 'a' || ch > 'z') &&
			(ch < 'A' || ch > 'Z') &&
			(ch < '0' || ch > '9')
		) {
			// Invalid character.
			return false;
		}
	}

	// Package valid, take it.
	m_package = package;
	return true;
}

bool FlexID::set_resource( const std::string& resource ) {
	// Package must be set.
	if( m_package.empty() ) {
		return false;
	}

	// Catch simple mistakes.
	if( resource.empty() ) {
		return false;
	}

	for( std::size_t ch_index = 0; ch_index < resource.size(); ++ch_index ) {
		char ch( resource[ch_index] );

		if(
			(ch < 'a' || ch > 'z') &&
			(ch < 'A' || ch > 'Z') &&
			(ch < '0' || ch > '9') &&
			ch != '_' &&
			ch != '-' &&
			ch != '.'
		) {
			// Invalid character.
			return false;
		}
	}

	// Resource valid, take it.
	m_resource = resource;
	return true;
}

std::string FlexID::as_path() const {
	assert( is_valid_package() == true );

	std::string path = get();

	for( std::size_t idx = 0; idx < path.size(); ++idx ) {
		if( path[idx] == '.' ) {
			path[idx] = '/';
		}
		else if( path[idx] == '/' ) {
			break;
		}
	}

	if( !is_valid_resource() ) {
		path += '/';
	}

	return path;
}

}
