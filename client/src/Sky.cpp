#include <GL/glew.h>

#include "Sky.hpp"
#include "Camera.hpp"

#include <FlexWorld/Math.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector3.hpp>
#include <vector>
#include <cmath>
#include <cassert>

Sky::Sky() :
	m_sun_texture( nullptr ),
	m_camera( nullptr ),
	m_sun_vbo( 0 ),
	m_sun_tbo( 0 ),
	m_sun_cbo( 0 ),
	m_stars_vbo( 0 ),
	m_stars_cbo( 0 ),
	m_radius( 5.0f ),
	m_time_of_day( 0.0f ),
	m_max_sun_height( 5.0f ),
	m_num_stars( 0 )
{
	set_sky_color( sf::Color( 0x47, 0x74, 0xcf ) );

	// Prepare sun.
	glGenBuffersARB( 1, &m_sun_vbo );
	glGenBuffersARB( 1, &m_sun_tbo );
	glGenBuffersARB( 1, &m_sun_cbo );

	sf::Vector3f vertices[4] = {
		sf::Vector3f( 0, 1, 0 ),
		sf::Vector3f( 0, 0, 0 ),
		sf::Vector3f( 1, 0, 0 ),
		sf::Vector3f( 1, 1, 0 )
	};

	sf::Vector2f tex_coords[4] = {
		sf::Vector2f( 0, 0 ),
		sf::Vector2f( 0, 1.f ),
		sf::Vector2f( 1.f, 1.f ),
		sf::Vector2f( 1.f, 0 )
	};

	sf::Color colors[4] = {
		sf::Color::White,
		sf::Color::White,
		sf::Color::White,
		sf::Color::White
	};

	glEnableClientState( GL_VERTEX_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, m_sun_vbo );
	glBufferData( GL_ARRAY_BUFFER, 4 * sizeof( sf::Vector3f ), vertices, GL_STATIC_DRAW );
	glDisableClientState( GL_VERTEX_ARRAY );

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, m_sun_tbo );
	glBufferData( GL_ARRAY_BUFFER, 4 * sizeof( sf::Vector2f ), tex_coords, GL_STATIC_DRAW );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glEnableClientState( GL_COLOR_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, m_sun_cbo );
	glBufferData( GL_ARRAY_BUFFER, 4 * sizeof( sf::Color::White ), &colors, GL_STATIC_DRAW );
	glDisableClientState( GL_COLOR_ARRAY );
}

Sky::~Sky() {
	glDeleteBuffersARB( 1, &m_sun_cbo );
	glDeleteBuffersARB( 1, &m_sun_tbo );
	glDeleteBuffersARB( 1, &m_sun_vbo );

	if( m_stars_vbo ) {
		glDeleteBuffersARB( 1, &m_stars_vbo );
	}

	if( m_stars_cbo ) {
		glDeleteBuffersARB( 1, &m_stars_cbo );
	}
}

void Sky::render() const {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glLoadIdentity();
	
	if( m_sun_texture ) {
		m_sun_texture->Bind();
	}

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Move sun depending on time of day.
	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
	}

	glTranslatef(
		m_radius * ((m_time_of_day - 0.5f) * 2.0f),
		std::sin( m_time_of_day * flex::PI ) * (m_radius / 2.0f),
		-m_radius
	);

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	// Render sun.
	glBindBufferARB( GL_ARRAY_BUFFER, m_sun_vbo );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );

	glBindBufferARB( GL_ARRAY_BUFFER, m_sun_tbo );
	glTexCoordPointer( 2, GL_FLOAT, 0, 0 );

	glBindBufferARB( GL_ARRAY_BUFFER, m_sun_cbo );
	glColorPointer( 4, GL_UNSIGNED_BYTE, 0, 0 );

	glDrawArrays( GL_QUADS, 0, 4 );

	// Render stars.
	glLoadIdentity();

	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
	}

	//glRotatef( -360.0f * m_time_of_day, 1.0f, 0.0f, 0.0f );

	if( m_num_stars > 0 ) {
		glBindBufferARB( GL_ARRAY_BUFFER, m_stars_vbo );
		glVertexPointer( 3, GL_FLOAT, 0, 0 );

		glBindBufferARB( GL_ARRAY_BUFFER, m_stars_cbo );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, 0 );

		glBindTexture( GL_TEXTURE_2D, 0 );

		glDrawArrays( GL_POINTS, 0, static_cast<GLuint>( m_num_stars ) );
	}

	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	glPopMatrix();
}

void Sky::set_camera( const Camera& camera ) {
	m_camera = &camera;
}

void Sky::set_sky_color( const sf::Color& color ) {
	m_sky_color = color;
}

const sf::Color& Sky::get_sky_color() const {
	return m_sky_color;
}

void Sky::set_sun_texture( const sf::Texture& texture ) {
	m_sun_texture = &texture;
}

void Sky::set_radius( float radius ) {
	m_radius = radius;
}

void Sky::set_time_of_day( float tod ) {
	m_time_of_day = std::min( 1.0f, std::max( 0.0f, tod ) );
}

float Sky::get_time_of_day() const {
	return m_time_of_day;
}

sf::Color Sky::get_local_sky_color() const {
	return sf::Color(
		static_cast<sf::Uint8>( std::sin( m_time_of_day * flex::PI ) * static_cast<float>( m_sky_color.r ) ),
		static_cast<sf::Uint8>( std::sin( m_time_of_day * flex::PI ) * static_cast<float>( m_sky_color.g ) ),
		static_cast<sf::Uint8>( std::sin( m_time_of_day * flex::PI ) * static_cast<float>( m_sky_color.b ) ),
		m_sky_color.a
	);
}

void Sky::generate_stars( std::size_t num_stars ) {
	assert( num_stars > 0 );

	m_num_stars = num_stars;

	if( !m_stars_vbo ) {
		glGenBuffersARB( 1, &m_stars_vbo );
	}

	if( !m_stars_cbo ) {
		glGenBuffersARB( 1, &m_stars_cbo );
	}

	std::vector<sf::Vector3f> star_vertices( num_stars, sf::Vector3f( 0, 0, 0 ) );
	std::vector<sf::Color> star_colors( num_stars, sf::Color::White );

	for( std::size_t star_idx = 0; star_idx < num_stars; ++star_idx ) {
		float phi = static_cast<float>( std::rand() * 360 ) * 180.0f / flex::PI;
		float theta = static_cast<float>( std::rand() * 360 ) * 180.0f / flex::PI;

		sf::Vector3f star_pos(
			m_radius * std::cos( theta ) * std::cos( phi ),
			m_radius * std::cos( theta ) * std::sin( phi ),
			m_radius * std::sin( theta )
		);

		star_vertices[star_idx] = star_pos;
	}

	// Set buffers.
	glEnableClientState( GL_VERTEX_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, m_stars_vbo );
	glBufferData( GL_ARRAY_BUFFER, num_stars * sizeof( sf::Vector3f ), &star_vertices[0], GL_STATIC_DRAW );
	glDisableClientState( GL_VERTEX_ARRAY );

	glEnableClientState( GL_COLOR_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, m_stars_cbo );
	glBufferData( GL_ARRAY_BUFFER, num_stars * sizeof( sf::Color ), &star_colors[0], GL_STATIC_DRAW );
	glDisableClientState( GL_COLOR_ARRAY );
}


