#pragma once

#include <SFML/Graphics.hpp>

/** FlexWorld client application.
 */
class Client {
	public:
		/** Ctor.
		 */
		Client();

		/** Run!
		 */
		void run();

	private:
		sf::RenderWindow m_window;
};
