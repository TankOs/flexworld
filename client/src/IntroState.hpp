#pragma once

#include "State.hpp"

/** Intro state.
 */
class IntroState : public State {
	public:
		/** Ctor.
		 * @param target Rendering target.
		 */
		IntroState( sf::RenderWindow& target );

	private:
		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( const sf::Time& delta );
		void render() const;

	private:
		sf::Time m_elapsed;
};
