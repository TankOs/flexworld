#include <FlexWorld/Triangle.hpp>

namespace flex {

Triangle::Triangle( VertexIndex v0, VertexIndex v1, VertexIndex v2 ) {
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
}

bool operator==( const Triangle& first, const Triangle& second ) {
	return
		first.vertices[0] == second.vertices[0] &&
		first.vertices[1] == second.vertices[1] &&
		first.vertices[2] == second.vertices[2]
	;
}

bool operator!=( const Triangle& first, const Triangle& second ) {
	return
		first.vertices[0] != second.vertices[0] ||
		first.vertices[1] != second.vertices[1] ||
		first.vertices[2] != second.vertices[2]
	;
}

}
