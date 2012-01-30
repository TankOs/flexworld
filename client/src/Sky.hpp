#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/OpenGL.hpp>

/** Sky.
 *
 * Renders horizon, stars, sun etc.
 */
class Sky {
	public:
		/** Ctor.
		 */
		Sky();

		/** Dtor.
		 */
		~Sky();

		/** Render.
		 */
		void render() const;

		/** Set sky color.
		 * @param color Color.
		 */
		void set_sky_color( const sf::Color& color );

		/** Get sky color.
		 * @return Sky color.
		 */
		const sf::Color& get_sky_color() const;

		/** Set sun texture.
		 * @param texture Texture.
		 */
		void set_sun_texture( const sf::Texture& texture );

	private:
		sf::Color m_sky_color;
		const sf::Texture* m_sun_texture;

		GLuint m_sun_vbo;
		GLuint m_sun_tbo;
		GLuint m_sun_cbo;
};
