#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace flex {

/** 3-dimensional vertex with normal and tex coords.
 */
struct Vertex {
	/** Ctor.
	 */
	Vertex();

	sf::Vector3f position; ///< Position vector.
	sf::Vector3f normal; ///< Normal vector.
	sf::Vector2f uv; ///< Texture coordinate.
};

}
