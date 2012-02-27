#include <FlexWorld/Mesh.hpp>

#include <limits>
#include <cassert>

namespace flex {

Mesh::Mesh() :
	m_texture_slot( 0 )
{
}

std::size_t Mesh::get_num_vertices() const {
	return m_vertices.size();
}

std::size_t Mesh::get_num_triangles() const {
	return m_triangles.size();
}

uint8_t Mesh::get_texture_slot() const {
	return m_texture_slot;
}

void Mesh::set_texture_slot( uint8_t slot ) {
	m_texture_slot = slot;
}

void Mesh::add_vertex( const sg::Vertex& vertex ) {
	assert( m_vertices.size() < std::numeric_limits<VertexIndex>::max() );

	m_vertices.push_back( vertex );
}

const sg::Vertex& Mesh::get_vertex( VertexIndex index ) const {
	assert( index < m_vertices.size() );

	return m_vertices[index];
}

void Mesh::define_triangle( const Triangle& triangle ) {
	assert( m_triangles.size() < std::numeric_limits<TriangleIndex>::max() );
	assert(
		triangle.vertices[0] != triangle.vertices[1] &&
		triangle.vertices[0] != triangle.vertices[2] &&
		triangle.vertices[1] != triangle.vertices[2]
	);
	assert(
		triangle.vertices[0] < m_vertices.size() &&
		triangle.vertices[1] < m_vertices.size() &&
		triangle.vertices[2] < m_vertices.size()
	);

	m_triangles.push_back( triangle );
}

const Triangle& Mesh::get_triangle( TriangleIndex index ) const {
	assert( index < m_triangles.size() );

	return m_triangles[index];
}

void Mesh::clear() {
	m_triangles.clear();
	m_vertices.clear();
	m_texture_slot = 0;
}

}
