#include <FlexWorld/Texture.hpp>

namespace flex {

Texture::Texture()
{
}

void Texture::set_path( const std::string& path ) {
	m_path = path;
}

const std::string& Texture::get_path() const {
	return m_path;
}

}
