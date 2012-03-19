#include <FlexWorld/Mesh.hpp>

#include <limits>
#include <cassert>

namespace flex {

Mesh::Mesh() :
	m_texture_slot( 0 )
{
}

sg::TriangleGeometry& Mesh::get_geometry() {
	return m_geometry;
}

const sg::TriangleGeometry& Mesh::get_geometry() const {
	return m_geometry;
}

uint8_t Mesh::get_texture_slot() const {
	return m_texture_slot;
}

void Mesh::set_texture_slot( uint8_t slot ) {
	m_texture_slot = slot;
}

void Mesh::clear() {
	m_geometry = sg::TriangleGeometry();
	m_texture_slot = 0;
}

}
