#pragma once

#include "State.hpp"

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
		void update( uint32_t delta );
		void render() const;

	private:
		uint32_t m_elapsed;
};
