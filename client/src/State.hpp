#pragma once

#include <SFML/System/Time.hpp>
#include <cstdint>

namespace sf {
class Event;
class RenderWindow;
}

/** State.
 */
class State {
	public:
		/** Ctor.
		 * @param target Render target.
		 */
		State( sf::RenderWindow& target );

		/** Dtor.
		 */
		virtual ~State();

		/** Run.
		 * @return Next state or nullptr.
		 */
		State* run();

	protected:
		/** Initialize.
		 */
		virtual void init() = 0;

		/** Cleanup.
		 */
		virtual void cleanup() = 0;

		/** Handle event.
		 * @param event Event.
		 */
		virtual void handle_event( const sf::Event& event ) = 0;

		/** Update logic.
		 * @param delta Time passed since last call to update().
		 */
		virtual void update( const sf::Time& delta ) = 0;

		/** Render.
		 */
		virtual void render() const = 0;

		/** Set render loop FPS.
		 * @param fps FPS. (0 to disable, useful for vsync)
		 */
		void set_render_fps( uint16_t fps );

		/** Leave state.
		 * @param next Next state (nullptr for none).
		 */
		void leave( State* next = nullptr );

		/** Get render target.
		 * @return Render target.
		 */
		sf::RenderWindow& get_render_target() const;

		/** Get rendering FPS.
		 * @return Rendering FPS.
		 */
		unsigned int get_render_fps() const;

	private:
		sf::RenderWindow& m_render_target;

		sf::Time m_render_interval;

		unsigned int m_render_fps;

		bool m_run;
		State* m_next_state;
};
