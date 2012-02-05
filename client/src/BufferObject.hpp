#pragma once

#include <FlexWorld/Vertex.hpp>
#include <FlexWorld/NonCopyable.hpp>

#include <SFML/OpenGL.hpp>
#include <vector>

/** Buffer object.
 * OpenGL buffer object for storing vertices, normals, texture coordinates and
 * optionally vertex indices.
 * The actual buffers are sent to OpenGL when render() is called, all other
 * calls are therefore delayed. This makes it possible to prepare a buffer when
 * not having a valid OGL context.
 * Once the data has been uploaded to OGL in render(), it is removed from RAM
 * and further calls to add_vertex() are NOT possible.
 */
class BufferObject : public flex::NonCopyable {
	public:
		/** Flags.
		 */
		enum Flag {
			VERTICES_ONLY = 0,
			NORMALS = 1 << 0,
			TEX_COORDS = 1 << 1,
			INDICES = 1 << 2
		};

		/** Ctor.
		 * @param flags Flags.
		 */
		BufferObject( int flags = VERTICES_ONLY );

		/** Dtor.
		 */
		~BufferObject();

		/** Add vertex.
		 * @param vertex Vertex.
		 */
		void add_vertex( const flex::Vertex& vertex );

		/** Upload data to GPU.
		 * The internal buffers are cleared, i.e. added vertices.
		 */
		void upload();

		/** Render.
		 * Make sure to upload() before.
		 */
		void render() const;

		/** Check if buffer object needs upload.
		 * @return true if upload is needed.
		 */
		bool is_upload_needed() const;

	private:
		typedef std::vector<sf::Vector3f> Vector3Array;
		typedef std::vector<sf::Vector2f> Vector2Array;

		enum {
			VBO_INDEX = 0,
			NBO_INDEX,
			TBO_INDEX,
			IBO_INDEX,
			NUM_BUFFERS
		};

		Vector3Array m_vertices;
		Vector3Array m_normals;
		Vector2Array m_tex_coords;

		std::size_t m_num_vertices;
		int m_flags;

		GLuint m_buffers[NUM_BUFFERS];
};
