#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/OpenGL.hpp>
#include <cstdint>

namespace sf {
class Texture;
}

class Camera;

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

		/** Set camera.
		 * @param camera Camera.
		 */
		void set_camera( const Camera& camera );

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

		/** Set radius (distance of sun and other effects)
		 * @param radius Radius.
		 */
		void set_radius( float radius );

		/** Set time of day.
		 * @param tod Time of day (0 = begin, 1 = end).
		 */
		void set_time_of_day( float tod );

		/** Get time of day.
		 * @return Time of day (0 = begin, 1 = end).
		 */
		float get_time_of_day() const;

		/** Get sky color depending on time of day.
		 * @return Sky color.
		 */
		sf::Color get_local_sky_color() const;

	private:
		sf::Color m_sky_color;
		const sf::Texture* m_sun_texture;
		const Camera* m_camera;

		GLuint m_sun_vbo;
		GLuint m_sun_tbo;
		GLuint m_sun_cbo;

		float m_radius;
		float m_time_of_day;
		float m_max_sun_height;
};
