#include "IntroState.hpp"
#include "MenuState.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>// XXX 

IntroState::IntroState( sf::RenderWindow& target ) :
	State( target )
{
}

void IntroState::init() {
}

void IntroState::cleanup() {
}

void IntroState::handle_event( const sf::Event& event ) {
	if(
		event.Type == sf::Event::Closed ||
		(event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::Escape)
	) {
		leave( new MenuState( get_render_target() ) );
	}
}

void IntroState::update( const sf::Time& delta ) {
	m_elapsed += delta;

	if( m_elapsed.AsMilliseconds() >= 100 ) {
		leave( new MenuState( get_render_target() ) );
	}
}

void IntroState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.Clear();

	sf::Text text( "FlexWorld" );
	window.Draw( text );

	window.Display();
}
