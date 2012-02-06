#include <FlexWorld/Math.hpp>

#include <cmath>

namespace flex {

void normalize( sf::Vector3f& vector ) {
	float length = std::sqrt( vector.x * vector.x + vector.y * vector.y + vector.z * vector.z );

	if( length > 0 ) {
		vector.x /= length;
		vector.y /= length;
		vector.z /= length;
	}
}

}
