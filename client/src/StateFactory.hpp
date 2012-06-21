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
		/** Create intro state.
		 * @param window Render window.
		 * @return Intro state.
		 */
		static State* create_intro_state( sf::RenderWindow& window );

		/** Create menu state.
		 * @param window Render window.
		 * @return Menu state.
		 */
		static State* create_menu_state( sf::RenderWindow& window );

		/** Create connect state.
		 * @param window Render window.
		 * @return Connect state.
		 */
		static State* create_connect_state( sf::RenderWindow& window );

		/** Create play state.
		 * @param window Render window.
		 * @return Play state.
		 */
		static State* create_play_state( sf::RenderWindow& window );
};
