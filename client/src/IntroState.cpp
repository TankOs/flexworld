#include "IntroState.hpp"
#include "MenuState.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

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
		event.type == sf::Event::Closed ||
		(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	) {
		leave( new MenuState( get_render_target() ) );
	}
}

void IntroState::update( const sf::Time& delta ) {
	m_elapsed += delta;

	if( m_elapsed.asMilliseconds() >= 100 ) {
		leave( new MenuState( get_render_target() ) );
	}
}

void IntroState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.clear();

	sf::Text text( "FlexWorld" );
	window.draw( text );

	window.display();
}
