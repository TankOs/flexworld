#pragma once

#include <FlexWorld/Vertex.hpp>
#include <FlexWorld/Triangle.hpp>

#include <vector>
#include <cstdint>

namespace flex {

/** 3D mesh.
 */
class Mesh {
	public:
		typedef uint8_t TextureSlot; ///< Texture slot.
		typedef Triangle::VertexIndex VertexIndex; ///< Vertex index type.
		typedef uint16_t TriangleIndex; ///< Triangle index type.

		/** Ctor.
		 */
		Mesh();

		/** Get number of vertices.
		 * @return Number of vertices.
		 */
		std::size_t get_num_vertices() const;

		/** Get number of triangles.
		 * @return Number of triangles.
		 */
		std::size_t get_num_triangles() const;

		/** Get texture slot.
		 * @return Texture slot.
		 */
		TextureSlot get_texture_slot() const;

		/** Set texture slot.
		 * @param slot Slot.
		 */
		void set_texture_slot( TextureSlot slot );

		/** Add vertex.
		 * A maximum of max( VertexIndex ) vertices is allowed.
		 * @param vertex Vertex.
		 */
		void add_vertex( const Vertex& vertex );

		/** Get vertex.
		 * Undefined behaviour if index out of range.
		 * @param index Index.
		 * @return Vertex.
		 */
		const Vertex& get_vertex( VertexIndex index ) const;

		/** Define a new triangle.
		 * Triangle indices must be valid and different from each other, otherwise
		 * behaviour is undefined. Also the maximum amount of max( TriangleIndex )
		 * shall not be exceeded.
		 * @param triangle Triangle.
		 */
		void define_triangle( const Triangle& triangle );

		/** Get triangle.
		 * Undefined behaviour if index is out of range.
		 * @param index Index.
		 * @return Triangle.
		 */
		const Triangle& get_triangle( TriangleIndex index ) const;

		/** Clear.
		 */
		void clear();

	private:
		typedef std::vector<Vertex> VertexVector;
		typedef std::vector<Triangle> TriangleVector;

		VertexVector m_vertices;
		TriangleVector m_triangles;

		TextureSlot m_texture_slot;
};

}
