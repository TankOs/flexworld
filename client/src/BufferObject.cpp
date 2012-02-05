#include <GL/glew.h>
#include "BufferObject.hpp"

#include <cstring>
#include <cassert>

BufferObject::BufferObject( int flags ) :
	m_num_vertices( 0 ),
	m_flags( flags )
{
	// Not yet supported, make it crash.
	assert( (m_flags & INDICES) == 0 );

	std::memset( m_buffers, 0, sizeof( GLuint ) * NUM_BUFFERS );
}

BufferObject::~BufferObject() {
	// Kill buffers.
	for( std::size_t buffer_idx = 0; buffer_idx < NUM_BUFFERS; ++buffer_idx ) {
		if( m_buffers[buffer_idx] != 0 ) {
			glDeleteBuffersARB( 1, &m_buffers[buffer_idx] );
		}
	}
}

void BufferObject::add_vertex( const flex::Vertex& vertex ) {
	assert( m_num_vertices == 0 );

	m_vertices.push_back( vertex.position );

	if( m_flags & NORMALS ) {
		m_normals.push_back( vertex.normal );
	}

	if( m_flags & TEX_COORDS ) {
		m_tex_coords.push_back( vertex.uv );
	}

}

void BufferObject::render() const {
	assert( m_num_vertices > 0 );

	// Enable client states and setup pointers.
	glEnableClientState( GL_VERTEX_ARRAY );
	glBindBufferARB( GL_ARRAY_BUFFER, m_buffers[VBO_INDEX] );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );

	if( (m_flags & NORMALS) == NORMALS ) {
		glEnableClientState( GL_NORMAL_ARRAY );
		glBindBufferARB( GL_ARRAY_BUFFER, m_buffers[NBO_INDEX] );
		glNormalPointer( GL_FLOAT, 0, 0 );
	}
	else {
		glDisableClientState( GL_NORMAL_ARRAY );
	}

	if( (m_flags & TEX_COORDS) == TEX_COORDS ) {
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glBindBufferARB( GL_ARRAY_BUFFER, m_buffers[TBO_INDEX] );
		glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
	}
	else {
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

	glDisableClientState( GL_COLOR_ARRAY );

	glDrawArrays( GL_TRIANGLES, 0, static_cast<GLint>( m_num_vertices ) );

	// Disable client states.
	glDisableClientState( GL_VERTEX_ARRAY );

	if( (m_flags & NORMALS) == NORMALS ) {
		glDisableClientState( GL_NORMAL_ARRAY );
	}

	if( (m_flags & TEX_COORDS) == TEX_COORDS ) {
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

}

void BufferObject::upload() {
	assert( is_upload_needed() );

	m_num_vertices = m_vertices.size();

	// Process VBO.
	if( m_buffers[VBO_INDEX] == 0 ) {
		glGenBuffersARB( 1, &m_buffers[VBO_INDEX] );
	}

	glBindBufferARB( GL_ARRAY_BUFFER, m_buffers[VBO_INDEX] );
	glBufferDataARB( GL_ARRAY_BUFFER, m_num_vertices * sizeof( Vector3Array::value_type ), &m_vertices[0], GL_STATIC_DRAW );

	// Normals.
	if( m_flags & NORMALS ) {
		if( m_buffers[NBO_INDEX] == 0 ) {
			glGenBuffersARB( 1, &m_buffers[NBO_INDEX] );
		}

		glBindBufferARB( GL_ARRAY_BUFFER, m_buffers[NBO_INDEX] );
		glBufferDataARB( GL_ARRAY_BUFFER, m_num_vertices * sizeof( Vector3Array::value_type ), &m_normals[0], GL_STATIC_DRAW );
	}

	// Texture coordinates.
	if( m_flags & TEX_COORDS ) {
		if( m_buffers[TBO_INDEX] == 0 ) {
			glGenBuffersARB( 1, &m_buffers[TBO_INDEX] );
		}

		glBindBufferARB( GL_ARRAY_BUFFER, m_buffers[TBO_INDEX] );
		glBufferDataARB( GL_ARRAY_BUFFER, m_num_vertices * sizeof( Vector2Array::value_type ), &m_tex_coords[0], GL_STATIC_DRAW );
	}

	// Unbind buffers.
	glBindBufferARB( GL_ARRAY_BUFFER, 0 );
	glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// Delete local data buffers.
	m_vertices.clear();
	m_normals.clear();
	m_tex_coords.clear();
}

bool BufferObject::is_upload_needed() const {
	return m_vertices.size() > 0;
}
