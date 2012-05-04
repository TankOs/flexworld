#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Text.hpp>
#include <list>

namespace sf {
class RenderTarget;
}

/** Text scrolling, mostly used for showing latest chat messages.
 *
 * An internal timer is used.
 */
class TextScroller {
	public:
		/** Ctor.
		 * @param bottom_left Bottom-left point.
		 */
		TextScroller( const sf::Vector2f& bottom_left );

		/** Set sustain duration.
		 * @param duration Duration.
		 */
		void set_sustain_duration( const sf::Time& duration );

		/** Set fade duration.
		 * @param duration Duration.
		 */
		void set_fade_duration( const sf::Time& fade_duration );

		/** Set maximum number of lines.
		 * @param num Maximum number of lines.
		 */
		void set_max_num_lines( std::size_t num );

		/** Add text.
		 * @param text Text.
		 */
		void add_text( const sf::String& text );

		/** Update.
		 */
		void update();

		/** Render.
		 * @param target Render target.
		 */
		void render( sf::RenderTarget& target ) const;

	private:
		struct Line {
			sf::Text text;
			sf::Text shadow;
			sf::Time creation_time;
		};

		typedef std::list<Line> LineList;

		void invalidate();

		LineList m_lines;

		sf::Clock m_timer;

		sf::Vector2f m_bottom_left;

		sf::Time m_sustain_duration;
		sf::Time m_fade_duration;

		std::size_t m_max_num_lines;
		float m_line_spacing;
		unsigned int m_font_size;

		const sf::Font* m_font;
};
