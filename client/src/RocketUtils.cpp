#include "RocketUtils.hpp"

#include <Rocket/Core.h>
#include <sstream>
#include <cmath>

void center_element( Rocket::Core::Element& element, const sf::Vector2f& center ) {
	// Sum up all box's sizes.
	sf::Vector2f element_size( 0.0f, 0.0f );

	for( int box_idx = 0; box_idx < element.GetNumBoxes(); ++box_idx ) {
		const Rocket::Core::Box& box = element.GetBox( box_idx );

		element_size.x += box.GetSize( Rocket::Core::Box::MARGIN ).x;
		element_size.y += box.GetSize( Rocket::Core::Box::MARGIN ).y;
	}

	{
		std::stringstream sstr;

		sstr << std::ceil( center.x - (element_size.x / 2.0f) );
		element.SetProperty( "left", sstr.str().c_str() );
	}
	{
		std::stringstream sstr;

		sstr << std::ceil( center.y - (element_size.y / 2.0f) );
		element.SetProperty( "top", sstr.str().c_str() );
	}
}
