#pragma once

#include <SFML/System/Vector2.hpp>

namespace Rocket {
namespace Core {
class Element;
}
}

/** Center an element at a point.
 * @param element Element.
 * @param center Center point.
 */
void center_element( Rocket::Core::Element& element, const sf::Vector2f& center );
