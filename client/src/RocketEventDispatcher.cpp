#include "RocketEventDispatcher.hpp"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include <Rocket/Core/Context.h>

int translate_mouse_button( sf::Mouse::Button button ) {
	int t_button = 0;

	switch( button ) {
		case sf::Mouse::Left:
			t_button = 0;
			break;

		case sf::Mouse::Right:
			t_button = 1;
			break;

		case sf::Mouse::Middle:
			t_button = 2;
			break;

		default:
			t_button = 0;
			break;
	}

	return t_button;
}

void RocketEventDispatcher::dispatch_event( const sf::Event& event, Rocket::Core::Context& context ) {
	// Mouse events.
	if( event.type == sf::Event::MouseMoved ) {
		context.ProcessMouseMove( event.mouseMove.x, event.mouseMove.y, 0 );
	}
	else if( event.type == sf::Event::MouseButtonPressed ) {
		context.ProcessMouseButtonDown( translate_mouse_button( event.mouseButton.button ), 0 );
	}
	else if( event.type == sf::Event::MouseButtonReleased ) {
		context.ProcessMouseButtonUp( translate_mouse_button( event.mouseButton.button ), 0 );
	}
}
