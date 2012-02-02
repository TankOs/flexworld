#pragma once

#include <cstdint>

namespace flex {

/** Simple structure for defining a triangle.
 * For vertices indices are used.
 */
struct Triangle {
	typedef uint16_t VertexIndex; ///< Vertex index type.

	/** Ctor.
	 * @param v0 Vertex 0 index.
	 * @param v1 Vertex 1 index.
	 * @param v2 Vertex 2 index.
	 */
	Triangle( VertexIndex v0 = 0, VertexIndex v1 = 0, VertexIndex v2 = 0 );

	VertexIndex vertices[3]; ///< Vertex indices.
};

/** Check for equal triangles.
 * @param first First triangle.
 * @param second Second triangle.
 * @return true if equal.
 */
bool operator==( const Triangle& first, const Triangle& second );

/** Check for unequal triangles.
 * @param first First triangle.
 * @param second Second triangle.
 * @return true if unequal.
 */
bool operator!=( const Triangle& first, const Triangle& second );

}
