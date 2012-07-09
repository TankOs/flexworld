#pragma once

#include <SFML/System/Vector3.hpp>

namespace fw {

/** Cuboid.
 */
template <class T>
struct Cuboid {
	typedef T Type; ///< Value type.

	/** Calculate intersection of two cuboids.
	 * @param first First cuboid.
	 * @param second Second cuboid.
	 * @return Intersection (width = height = depth = 0 if cuboids do not intersect).
	 */
	static Cuboid<T> calc_intersection( const Cuboid<T>& first, const Cuboid<T>& second );

	/** Ctor.
	 * @param x_ X.
	 * @param y_ Y.
	 * @param z_ Z.
	 * @param width_ Width.
	 * @param height_ Height.
	 * @param depth_ Depth.
	 */
	Cuboid( T x_ = 0, T y_ = 0, T z_ = 0, T width_ = 0, T height_ = 0, T depth_ = 0 );

	/** Check if cuboid contains a point.
	 * @param point Point.
	 * @return true if cuboid contains point.
	 */
	bool contains( const sf::Vector3<T>& point ) const;

	T x; ///< X.
	T y; ///< Y.
	T z; ///< Z.
	T width; ///< Width.
	T height; ///< Height.
	T depth; ///< Depth.
};

typedef Cuboid<float> FloatCuboid; ///< Float cuboid.

/** Test equality.
 * @param first First cuboid.
 * @param second Second cuboid.
 * @return true if equal.
 */
template <class T>
bool operator==( const Cuboid<T>& first, const Cuboid<T>& second );

/** Test unequality.
 * @param first First cuboid.
 * @param second Second cuboid.
 * @return true if unequal.
 */
template <class T>
bool operator!=( const Cuboid<T>& first, const Cuboid<T>& second );

}

#include "Cuboid.inl"
