#include <GL/glew.h>

#include "PlanetRenderer.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Model.hpp>
#include <FlexWorld/Class.hpp>

#include <SFML/System/Clock.hpp>
#include <cassert>
#include <iostream> // XXX 

static const float GRID_SIZE = 1.0f;

PlanetRenderer::PlanetRenderer( const flex::Planet& planet, ResourceManager& resource_manager ) :
	m_camera( nullptr ),
	m_planet( planet ),
	m_resource_manager( resource_manager )
{
}

PlanetRenderer::~PlanetRenderer() {
}

void PlanetRenderer::prepare_chunk( const flex::Planet::Vector& chunk_pos ) {
	typedef std::map<std::shared_ptr<const sf::Texture>, std::shared_ptr<BufferObject>> TextureVBOMap;

	sf::Clock clock;

	const flex::Chunk::Vector& chunk_size = m_planet.get_chunk_size();

	sf::Vector3f org_offset(
		static_cast<float>( chunk_pos.x * chunk_size.x ) * GRID_SIZE,
		static_cast<float>( chunk_pos.y * chunk_size.y ) * GRID_SIZE,
		static_cast<float>( chunk_pos.z * chunk_size.z ) * GRID_SIZE
	);
	sf::Vector3f offset = org_offset;

	const flex::Class* block_cls = nullptr;
	std::size_t vertex_idx = 0;
	flex::Vertex vertex;
	TextureVBOMap new_vbos;

	flex::Chunk::Vector block_runner;
	for( block_runner.z = 0; block_runner.z < chunk_size.z; ++block_runner.z ) {
		for( block_runner.y = 0; block_runner.y < chunk_size.y; ++block_runner.y ) {
			for( block_runner.x = 0; block_runner.x < chunk_size.x; ++block_runner.x ) {
				// Skip empty blocks.
				block_cls = m_planet.find_block( chunk_pos, block_runner );

				if( block_cls != nullptr ) {
					// Get model.
					const flex::FlexID& model_id = block_cls->get_model().get_id();
					std::shared_ptr<const flex::Model> model = m_resource_manager.find_model( model_id );

					if( model == nullptr ) {
						// Try to load.
						if( !m_resource_manager.load_model( model_id ) ) {
#if !defined( NDEBUG )
							std::cerr << "Failed to load model: " << model_id.get() << std::endl;
#endif
							return;
							// TODO Exception?
						}

						model = m_resource_manager.find_model( model_id );
						assert( model );
					}

					// Iterate over meshes.
					for( std::size_t mesh_idx = 0; mesh_idx < model->get_num_meshes(); ++mesh_idx ) {
						const flex::Mesh& mesh = model->get_mesh( mesh_idx );

						assert( mesh.get_texture_slot() < block_cls->get_num_textures() );

						// Get texture.
						const flex::FlexID& texture_id = block_cls->get_texture( mesh.get_texture_slot() ).get_id();
						std::shared_ptr<const sf::Texture> texture = m_resource_manager.find_texture( texture_id );

						if( texture == nullptr ) {
							// Try to load.
							if( !m_resource_manager.load_texture( texture_id ) ) {
#if !defined( NDEBUG )
								std::cerr << "Failed to load texture: " << texture_id.get() << std::endl;
#endif
								return;
							}

							texture = m_resource_manager.find_texture( texture_id );
							assert( texture );
						}

						// Get VBO or generate a new one.
						std::shared_ptr<BufferObject>& vbo = new_vbos[texture];

						if( vbo == nullptr ) {
							vbo.reset( new BufferObject( BufferObject::TEX_COORDS | BufferObject::NORMALS ) );
						}

						// Iterate over triangles.
						for( flex::Mesh::TriangleIndex tri_idx = 0; tri_idx < mesh.get_num_triangles(); ++tri_idx ) {
							const flex::Triangle& tri = mesh.get_triangle( tri_idx );

							// Add triangle's vertices to VBO.
							for( vertex_idx = 0; vertex_idx < 3; ++vertex_idx ) {
								vertex = mesh.get_vertex( tri.vertices[vertex_idx] );

								vertex.position += offset;
								vbo->add_vertex( vertex );
							}
						}
					}
				}

				offset.x += GRID_SIZE;
			}

			offset.y += GRID_SIZE;
			offset.x = org_offset.x;
		}

		offset.z += GRID_SIZE;
		offset.y = org_offset.y;
	}

	// Calculate absolute chunk position.
	uint32_t abs_chunk_pos = (
		chunk_pos.z * (m_planet.get_size().x * m_planet.get_size().y) +
		chunk_pos.y * (m_planet.get_size().x) +
		chunk_pos.x
	);

	// Pause rendering as we're modifying variables that are used by the render()
	// func.
	pause();

	//
	// Apply VBOs to main rendering pool.
	//

	// Remove all VBOs that were created for this chunk before.
	ChunkTextureMap::iterator chunk_tex_iter = m_chunk_textures.find( abs_chunk_pos );

	if( chunk_tex_iter != m_chunk_textures.end() ) {
		TextureVBOIndexMap::iterator vbo_iter = chunk_tex_iter->second.begin();
		TextureVBOIndexMap::iterator vbo_iter_end = chunk_tex_iter->second.end();

		for( ; vbo_iter != vbo_iter_end; ++vbo_iter ) {
			m_vbos[vbo_iter->first][vbo_iter->second].reset();
		}

		m_chunk_textures.erase( chunk_tex_iter );
	}
	
	// Add prepared VBOs to main pool.
	TextureVBOMap::iterator new_vbo_iter = new_vbos.begin();
	TextureVBOMap::iterator new_vbo_iter_end = new_vbos.end();
	std::size_t vbo_idx = 0;

	for( ; new_vbo_iter != new_vbo_iter_end; ++new_vbo_iter ) {
		// Get reference to VBO pool for a specific texture.
		VBOVector& vbos = m_vbos[new_vbo_iter->first];

		// Search for free slot.
		for( vbo_idx = 0; vbo_idx < vbos.size(); ++vbo_idx ) {
			if( vbos[vbo_idx] == nullptr ) {
				break;
			}
		}

		// Check if VBO pool needs to be enlarged.
		if( vbo_idx >= vbos.size() ) {
			vbos.resize( vbos.size() + 1 );
		}

		// Save VBO and create link between chunk/texture -> VBO.
		vbos[vbo_idx] = new_vbo_iter->second;
		m_chunk_textures[abs_chunk_pos][new_vbo_iter->first] = vbo_idx;
	}

	resume();

	//std::cout << clock.GetElapsedTime().AsMicroseconds() << " µs" << std::endl;
}

void PlanetRenderer::render() const {
	boost::lock_guard<boost::mutex> lock( m_render_mutex );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glColor3f( 1, 1, 1 );

	// Apply camera.
	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
		glTranslatef( -m_camera->get_position().x, -m_camera->get_position().y, -m_camera->get_position().z );
	}

	glTranslatef( -50, -2.70f, -50 );

	std::size_t vbo_idx = 0;
	std::size_t num_vbos = 0;
	const VBOVector* vbos;
	std::shared_ptr<BufferObject> buffer;

	TextureVBOVectorMap::const_iterator tex_vbo_iter( m_vbos.begin() );
	TextureVBOVectorMap::const_iterator tex_vbo_iter_end( m_vbos.end() );

	for( ; tex_vbo_iter != tex_vbo_iter_end; ++tex_vbo_iter ) {
		vbos = &tex_vbo_iter->second;
		num_vbos = vbos->size();

		// Bind texture.
		tex_vbo_iter->first->Bind();

		for( vbo_idx = 0; vbo_idx < num_vbos; ++vbo_idx ) {
			buffer = (*vbos)[vbo_idx];

			// Skip free VBO slots.
			if( buffer == nullptr ) {
				continue;
			}

			// Upload data to GPU if needed.
			if( buffer->is_upload_needed() ) {
				buffer->upload();
			}

			// Render VBO.
			buffer->render();
		}
	}
}

void PlanetRenderer::pause() {
	m_render_mutex.lock();
}

void PlanetRenderer::resume() {
	m_render_mutex.unlock();
}

void PlanetRenderer::set_camera( const Camera& camera ) {
	m_camera = &camera;
}
