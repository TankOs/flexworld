#pragma once

class State;

namespace sf {
class RenderWindow;
}

/** Factory for creating states.
 * The factory is mainly used to reduce compilation times.
 */
class StateFactory {
	public:
		static State* create_intro_state( sf::RenderWindow& window );
		static State* create_menu_state( sf::RenderWindow& window );
		static State* create_connect_state( sf::RenderWindow& window );
		static State* create_play_state( sf::RenderWindow& window );
};
