#include <GL/glew.h>

#include "Sky.hpp"

#include <SFML/System/Vector3.hpp>
#include <iostream> // XXX 

Sky::Sky() :
	m_sun_texture( nullptr )
{
	set_sky_color( sf::Color( 0x47, 0x74, 0xcf ) );

	// Prepare sun.
	glGenBuffersARB( 1, &m_sun_vbo );
	glGenBuffersARB( 1, &m_sun_tbo );
	glGenBuffersARB( 1, &m_sun_cbo );

	sf::Vector3f vertices[4] = {
		sf::Vector3f( 0, 5, 0 ),
		sf::Vector3f( 0, 0, 0 ),
		sf::Vector3f( 5, 0, 0 ),
		sf::Vector3f( 5, 5, 0 )
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
}

void Sky::render() const {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glLoadIdentity();
	glTranslatef( 0, 0, -10.f );
	
	if( m_sun_texture ) {
		m_sun_texture->Bind();
	}

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	glBindBufferARB( GL_ARRAY_BUFFER, m_sun_vbo );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );

	glBindBufferARB( GL_ARRAY_BUFFER, m_sun_tbo );
	glTexCoordPointer( 2, GL_FLOAT, 0, 0 );

	glBindBufferARB( GL_ARRAY_BUFFER, m_sun_cbo );
	glColorPointer( 4, GL_UNSIGNED_BYTE, 0, 0 );

	glDrawArrays( GL_QUADS, 0, 4 );

	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	/*glBegin( GL_QUADS ); {
		glColor3f( 1, 1, 1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 1, 0, 0 );
		glVertex3f( 1, 1, 0 );
		glVertex3f( 0, 1, 0 );
	} glEnd();*/

	glPopMatrix();
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
