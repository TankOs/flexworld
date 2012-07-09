#include <FlexWorld/Version.hpp>

#include <sstream>

namespace fw {

Version::Version( uint32_t major, uint32_t minor, uint32_t revision ) :
	m_major( major ),
	m_minor( minor ),
	m_revision( revision )
{
}

uint32_t Version::get_major() const {
	return m_major;
}

uint32_t Version::get_minor() const {
	return m_minor;
}

uint32_t Version::get_revision() const {
	return m_revision;
}

void Version::set_major( uint32_t major ) {
	m_major = major;
}

void Version::set_minor( uint32_t minor ) {
	m_minor = minor;
}

void Version::set_revision( uint32_t revision ) {
	m_revision = revision;
}

bool operator==( const Version& left, const Version& right ) {
	return left.get_major() == right.get_major() && left.get_minor() == right.get_minor() && left.get_revision() == right.get_revision();
}

bool operator!=( const Version& left, const Version& right ) {
	return !(left == right);
}

bool operator>( const Version& left, const Version& right ) {
	if( left.get_major() > right.get_major() ) {
		return true;
	}
	else if( left.get_major() < right.get_major() ) {
		return false;
	}

	if( left.get_minor() > right.get_minor() ) {
		return true;
	}
	else if( left.get_minor() < right.get_minor() ) {
		return false;
	}

	if( left.get_revision() > right.get_revision() ) {
		return true;
	}

	return false;
}

bool operator<( const Version& left, const Version& right ) {
	return !(left > right) && !(left == right);
}

}
