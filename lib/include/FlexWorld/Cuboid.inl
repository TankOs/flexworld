#include <algorithm>

namespace flex {

template <class T>
Cuboid<T> Cuboid<T>::calc_intersection( const Cuboid<T>& first, const Cuboid<T>& second ) {
	Cuboid<T> intersection( 0, 0, 0, 0, 0, 0 );

	// Calculate boundaries.
	T left   = std::max( first.x, second.x );
	T bottom = std::max( first.y, second.y );
	T far    = std::max( first.z, second.z );
	T right  = std::min( first.x + first.width, second.x + second.width );
	T top    = std::min( first.y + first.height, second.y + second.height );
	T near   = std::min( first.z + first.depth, second.z + second.depth );

	if(
		(left < right) &&
		(bottom < top) &&
		(far < near)
	) {
		intersection.x = left;
		intersection.y = bottom;
		intersection.z = far;
		intersection.width = right - left;
		intersection.height = top - bottom;
		intersection.depth = near - far;
	}

	return intersection;
}

template <class T>
Cuboid<T>::Cuboid( T x_, T y_, T z_, T width_, T height_, T depth_ ) :
	x( x_ ),
	y( y_ ),
	z( z_ ),
	width( width_ ),
	height( height_ ),
	depth( depth_ )
{
}

template <class T>
bool operator==( const Cuboid<T>& first, const Cuboid<T>& second ) {
	return (
		first.x == second.x &&
		first.y == second.y &&
		first.z == second.z &&
		first.width == second.width &&
		first.height == second.height &&
		first.depth == second.depth
	);
}

template <class T>
bool operator!=( const Cuboid<T>& first, const Cuboid<T>& second ) {
	return (
		first.x != second.x ||
		first.y != second.y ||
		first.z != second.z ||
		first.width != second.width ||
		first.height != second.height ||
		first.depth != second.depth
	);
}

}
