#include "RocketEventDispatcher.hpp"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Utf.hpp>
#include <Rocket/Core.h>
#include <iostream> // XXX 

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

int translate_key( sf::Keyboard::Key key ) {
	return 0;
}

void RocketEventDispatcher::dispatch_event( const sf::Event& event, Rocket::Core::Context& context ) {
	// Translate SFML event to libRocket event.

	if( event.type == sf::Event::MouseMoved ) {
		context.ProcessMouseMove( event.mouseMove.x, event.mouseMove.y, 0 );
	}
	else if( event.type == sf::Event::MouseButtonPressed ) {
		context.ProcessMouseButtonDown( translate_mouse_button( event.mouseButton.button ), 0 );
	}
	else if( event.type == sf::Event::MouseButtonReleased ) {
		context.ProcessMouseButtonUp( translate_mouse_button( event.mouseButton.button ), 0 );
	}
	else if( event.type == sf::Event::MouseWheelMoved ) {
		context.ProcessMouseWheel( -event.mouseWheel.delta, 0 );
	}
	else if( event.type == sf::Event::TextEntered ) {
		// Convert UTF-32 to UTF-16 (UCS-2 is needed by libRocket).
		sf::Uint32 ch = event.text.unicode;
		sf::Uint16 rocket_ch = 0;

		sf::Utf<16>::encode( ch, &rocket_ch );

		context.ProcessTextInput( rocket_ch );
	}
	else if( event.type == sf::Event::KeyPressed ) {
	}
}
