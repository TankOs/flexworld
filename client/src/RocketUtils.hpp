#pragma once

#include <SFML/System/Vector2.hpp>

namespace Rocket {
namespace Core {
class Element;
}
}

namespace sf {
class String;
}

/** Center an element at a point.
 * @param element Element.
 * @param center Center point.
 */
void center_element( Rocket::Core::Element& element, const sf::Vector2f& center );

/** Escape an RML string.
 * Escaped strings can be safely set with SetInnerRML().
 * @param rml_string RML string.
 * @return Escaped string.
 */
sf::String escape_rml_string( const sf::String& rml_string );
