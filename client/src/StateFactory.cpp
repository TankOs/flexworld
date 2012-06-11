#include "StateFactory.hpp"
#include "IntroState.hpp"
#include "MenuState.hpp"
#include "ConnectState.hpp"
#include "PlayState.hpp"

State* StateFactory::create_intro_state( sf::RenderWindow& window ) {
	return new IntroState( window );
}

State* StateFactory::create_menu_state( sf::RenderWindow& window ) {
	return new MenuState( window );
}

State* StateFactory::create_connect_state( sf::RenderWindow& window ) {
	return new ConnectState( window );
}

State* StateFactory::create_play_state( sf::RenderWindow& window ) {
	return new PlayState( window );
}
