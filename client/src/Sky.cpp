#include <GL/glew.h>

#include "Sky.hpp"
#include "Camera.hpp"

#include <FlexWorld/Math.hpp>

#include <FWSG/TriangleGeometry.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector3.hpp>
#include <vector>
#include <cmath>
#include <cassert>

Sky::Sky() :
	m_sun_bo( sg::BufferObject::TEX_COORDS, false ),
	m_sun_texture( nullptr ),
	m_camera( nullptr ),
	m_radius( 100.0f ),
	m_time_of_day( 0.0f ),
	m_max_sun_height( 5.0f )
{
	set_sky_color( sf::Color( 0x47, 0x74, 0xcf ) );

	// Prepare sun buffer object.
	sg::TriangleGeometry geo;

	geo.add_triangle(
		sg::Vertex( sf::Vector3f( 0, 0, 0 ), sf::Vector3f( 0, 0, 0 ), sf::Vector2f( 0, 1 ) ),
		sg::Vertex( sf::Vector3f( 25, 0, 0 ), sf::Vector3f( 0, 0, 0 ), sf::Vector2f( 1, 1 ) ),
		sg::Vertex( sf::Vector3f( 25, 25, 0 ), sf::Vector3f( 0, 0, 0 ), sf::Vector2f( 1, 0 ) )
	);
	geo.add_triangle(
		sg::Vertex( sf::Vector3f( 0, 0, 0 ), sf::Vector3f( 0, 0, 0 ), sf::Vector2f( 0, 1 ) ),
		sg::Vertex( sf::Vector3f( 25, 25, 0 ), sf::Vector3f( 0, 0, 0 ), sf::Vector2f( 1, 0 ) ),
		sg::Vertex( sf::Vector3f( 0, 25, 0 ), sf::Vector3f( 0, 0, 0 ), sf::Vector2f( 0, 0 ) )
	);

	m_sun_bo.load( geo );
}

Sky::~Sky() {
}

void Sky::render() const {
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Total x rotation.
	float x_rot = m_time_of_day * 360.0f;
	float y_rot = 0.0f;

	// Move sun depending on time of day.
	glRotatef( x_rot, 1.0f, 0.0f, 0.0f );
	glRotatef( y_rot, 0.0f, 1.0f, 0.0f );

	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
	}

	glTranslatef(
		0,
		0,
		-m_radius
	);

	// Render sun.
	if( m_sun_texture ) {
		m_sun_texture->Bind();
	}

	glColor3f( 1, 1, 1 );

	m_sun_bo.render();

	// Render test floor.
	/*glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, 0 );

	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
	}

	glBegin( GL_QUADS ); {
		glColor3f( 1, 1, 1 );
		glVertex3f( -20.0f, -1.0f, -20.0f );
		glVertex3f( -20.0f, -1.0f, 20.0f );
		glVertex3f( 20.0f, -1.0f, 20.0f );
		glVertex3f( 20.0f, -1.0f, -20.0f );
	} glEnd();*/
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
		static_cast<sf::Uint8>( std::sin( (m_time_of_day + 0.2f) * flex::PI ) * static_cast<float>( m_sky_color.r ) ),
		static_cast<sf::Uint8>( std::sin( (m_time_of_day + 0.2f) * flex::PI ) * static_cast<float>( m_sky_color.g ) ),
		static_cast<sf::Uint8>( std::sin( (m_time_of_day + 0.2f) * flex::PI ) * static_cast<float>( m_sky_color.b ) ),
		m_sky_color.a
	);
}
