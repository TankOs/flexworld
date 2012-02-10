namespace flex {

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
