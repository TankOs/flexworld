#include <FlexWorld/FlexID.hpp>

namespace flex {

FlexID::FlexID() {
}

bool FlexID::parse( const std::string& id ) {
	if( id.empty() ) {
		return false;
	}

	// Check for valid beginning.
	if( id[0] == '.'  || id[0] == '/' ) {
		return false;
	}

	// Parse package ID.
	std::string::size_type package_end( 0 );

	while( package_end < id.size() ) {
		const char& ch( id[package_end] );

		// Check for path delimiter.
		if( ch == '/' ) {
			break;
		}

		// Check for valid character.
		if(
			(ch < 'a' || ch > 'z') &&
			(ch < 'A' || ch > 'Z') &&
			(ch < '0' || ch > '9') &&
			ch != '_' &&
			ch != '.'
		) {
			return false;
		}

		++package_end;
	}

	// Check for valid range.
	if( package_end == 0 || package_end >= id.size() || id[package_end] != '/' ) {
		return false;
	}

	// No path?
	if( package_end + 1 >= id.size() ) {
		return false;
	}

	m_package = id.substr( 0, package_end );
	m_resource = id.substr( package_end + 1 );

	return true;
}

const std::string& FlexID::get_package() const {
	return m_package;
}

const std::string& FlexID::get_resource() const {
	return m_resource;
}

std::string FlexID::get() const {
	if( !is_valid() ) {
		return "";
	}
	else if( m_resource.empty() ) {
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

bool FlexID::is_valid() const {
	return m_package.size() > 0;
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

}
