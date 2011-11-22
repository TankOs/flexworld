#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>

/** Utility class for morphing one color to another (linear).
 */
class ColorMorpher {
	public:
		/** Ctor.
		 */
		ColorMorpher();

		/** Start morphing.
		 * @param from_color From this color...
		 * @param to_color ...to this color.
		 * @param duration Morph duration.
		 */
		void start( const sf::Color& from_color, const sf::Color& to_color, uint32_t duration );

		/** Update.
		 * @param delta_t Time to calculate.
		 */
		void update( uint32_t delta_t );

		/** Get current color.
		 * @return Current Color.
		 */
		const sf::Color& get_current_color() const;

		/** Reset (stops the morph).
		 */
		void reset();

	private:
		float m_steps[3];
		float m_to_color[3];
		float m_current_color_calc[3];

		sf::Color m_current_color;
};
