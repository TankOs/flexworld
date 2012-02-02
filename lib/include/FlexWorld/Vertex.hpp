#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace flex {

/** 3-dimensional vertex with normal and tex coords.
 */
struct Vertex {
	/** Ctor.
	 * @param position_ Position.
	 * @param normal_ Normal.
	 * @param uv_ Texture coordinate.
	 */
	Vertex(
		const sf::Vector3f& position_ = sf::Vector3f( 0, 0, 0 ),
		const sf::Vector3f& normal_ = sf::Vector3f( 0, 1, 0 ),
		const sf::Vector2f& uv_ = sf::Vector2f( 0, 0 )
	);

	sf::Vector3f position; ///< Position vector.
	sf::Vector3f normal; ///< Normal vector.
	sf::Vector2f uv; ///< Texture coordinate.
};

/** Check for equal vertices.
 * @param first First vertex.
 * @param second Second vertex.
 * @return true if equal.
 */
bool operator==( const Vertex& first, const Vertex& second );

/** Check for unequal vertices.
 * @param first First vertex.
 * @param second Second vertex.
 * @return true if unequal.
 */
bool operator!=( const Vertex& first, const Vertex& second );

}
