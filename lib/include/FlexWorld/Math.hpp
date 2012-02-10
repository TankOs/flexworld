#pragma once

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>

namespace flex {

static const float PI = 3.14159265f;

/** Normalize vector.
 * @param vector Vector.
 */
void normalize( sf::Vector3f& vector );

/** Calculate area of triangle.
 * The points must be ordered counter-clockwise.
 * @param p0 Point 0.
 * @param p1 Point 1.
 * @param p2 Point 2.
 * @return Area.
 */
float calc_triangle_area( const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2 );

/** Calculate area of rect.
 * @param a Length of side A.
 * @param b Length of side B.
 * @return Area.
 */
float calc_rect_area( float a, float b );

}
