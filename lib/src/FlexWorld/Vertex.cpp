#include <FlexWorld/Vertex.hpp>

namespace flex {

Vertex::Vertex(
	const sf::Vector3f& position_,
	const sf::Vector3f& normal_,
	const sf::Vector2f& uv_
) :
	position( position_ ),
	normal( normal_ ),
	uv( uv_ )
{
}

bool operator==( const Vertex& first, const Vertex& second ) {
	return
		first.position == second.position &&
		first.normal == second.normal &&
		first.uv == second.uv
	;
}

bool operator!=( const Vertex& first, const Vertex& second ) {
	return
		first.position != second.position ||
		first.normal != second.normal ||
		first.uv != second.uv
	;
}


}
