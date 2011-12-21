#include <FlexWorld/FlexId.hpp>

namespace flex {

FlexId::FlexId( const std::string& id ) {
	if( !parse( id ) ) {
		throw ParserException( "Resource ID parser error: " + id );
	}
}

bool FlexId::parse( const std::string& id ) {
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
	m_path = id.substr( package_end + 1 );

	return true;
}

const std::string& FlexId::get_package_id() const {
	return m_package;
}

const std::string& FlexId::get_path() const {
	return m_path;
}

std::string FlexId::get() const {
	return m_package + "/" + m_path;
}

bool FlexId::operator==( const FlexId& other ) const {
	return m_package == other.m_package && m_path == other.m_path;
}

bool FlexId::operator!=( const FlexId& other ) const {
	return m_package != other.m_package || m_path != other.m_path;
}

// ---

FlexId::ParserException::ParserException( const std::string& msg ) :
	std::runtime_error( msg )
{
}

}
