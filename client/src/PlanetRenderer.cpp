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

	flex::Chunk::Vector block_runner;
	flex::Chunk::Vector chunk_size = m_planet.get_chunk_size();
	const flex::Class* block_cls = nullptr;
	sf::Vector3f org_offset(
		static_cast<float>( chunk_pos.x * m_planet.get_chunk_size().x ) * GRID_SIZE,
		static_cast<float>( chunk_pos.y * m_planet.get_chunk_size().y ) * GRID_SIZE,
		static_cast<float>( chunk_pos.z * m_planet.get_chunk_size().z ) * GRID_SIZE
	);
	sf::Vector3f offset = org_offset;
	TextureVBOMap vbos;
	std::size_t vertex_idx = 0;
	flex::Vertex vertex;

	for( block_runner.z = 0; block_runner.z < chunk_size.z; ++block_runner.z ) {
		for( block_runner.y = 0; block_runner.y < chunk_size.y; ++block_runner.y ) {
			for( block_runner.x = 0; block_runner.x < chunk_size.x; ++block_runner.x ) {
				// Skip empty blocks.
				block_cls = m_planet.find_block( chunk_pos, block_runner );

				if( block_cls == nullptr ) {
					continue;
				}

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
					std::shared_ptr<BufferObject>& vbo = vbos[texture];

					if( vbo == nullptr ) {
						vbo.reset( new BufferObject( BufferObject::VERTICES_ONLY ) );
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

	TextureVBOMap::iterator tex_vbo_iter( vbos.begin() );
	TextureVBOMap::iterator tex_vbo_iter_end( vbos.end() );
	std::size_t vbo_idx = 0;
	
	for( ; tex_vbo_iter != tex_vbo_iter_end; ++tex_vbo_iter ) {
		// Get old index if any.
		vbo_idx = m_vbos.size();

		// Check if this chunk had a VBO before. If so, remove it from the VBO array.
		{
			TextureChunkPositionMap::iterator tex_pos_iter = m_chunk_positions.find( tex_vbo_iter->first );

			if( tex_pos_iter != m_chunk_positions.end() ) {
				ChunkVBOIndexMap::iterator pos_vbo_iter = tex_pos_iter->second.find( abs_chunk_pos );

				if( pos_vbo_iter != tex_pos_iter->second.end() ) {
					// Save old index, so it's being reused.
					vbo_idx = pos_vbo_iter->second;

					// Reset the previous VBO.
					m_vbos[vbo_idx].reset();

					// Do not remove link (doesn't have to be done, gets overwritten anyways).
					//tex_pos_iter->second.erase( pos_vbo_iter );
				}
			}
		}

		// Check if the VBO vbo_idx is pointing to can be reused.
		if( vbo_idx >= m_vbos.size() || m_vbos[vbo_idx] != nullptr ) {
			// Can't be reused, find next free slot.
			for( vbo_idx = 0; vbo_idx < m_vbos.size(); ++vbo_idx ) {
				if( m_vbos[vbo_idx] == nullptr ) {
					break;
				}
			}
		}

		// If no free slot found, create a new one.
		if( vbo_idx >= m_vbos.size() ) {
			m_vbos.push_back( std::shared_ptr<BufferObject>() );
		}

		assert( vbo_idx < m_vbos.size() );

		// Save VBO and link.
		m_vbos[vbo_idx] = tex_vbo_iter->second;
		m_chunk_positions[tex_vbo_iter->first][abs_chunk_pos] = vbo_idx;
	}

	resume();

	//std::cout << clock.GetElapsedTime().AsMicroseconds() << " µs" << std::endl;
}

void PlanetRenderer::render() const {
	boost::lock_guard<boost::mutex> lock( m_render_mutex );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glBindTexture( GL_TEXTURE_2D, 0 );

	glColor3f( 1, 1, 1 );

	// Apply camera.
	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
	}

	glTranslatef( -50, -3, -50 );

	std::size_t vbo_idx = 0;
	std::size_t num_vbos = m_vbos.size();

	for( ; vbo_idx < num_vbos; ++vbo_idx ) {
		if( m_vbos[vbo_idx] == nullptr ) {
			continue;
		}

		// Upload data to GPU if needed.
		if( m_vbos[vbo_idx]->is_upload_needed() ) {
			m_vbos[vbo_idx]->upload();
		}

		m_vbos[vbo_idx]->render();
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
