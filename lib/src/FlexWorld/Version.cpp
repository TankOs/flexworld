#include <FlexWorld/Version.hpp>

namespace flex {

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

}
