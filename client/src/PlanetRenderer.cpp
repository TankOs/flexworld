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
	m_planet( planet ),
	m_resource_manager( resource_manager )
{
}

PlanetRenderer::~PlanetRenderer() {
	for( std::size_t vbo_idx = 0; vbo_idx < m_vbos.size(); ++vbo_idx ) {
		if( m_vbos[vbo_idx] != 0 ) {
			glDeleteBuffersARB( 1, &m_vbos[vbo_idx] );
		}
	}
}

void PlanetRenderer::prepare_chunk( const flex::Planet::Vector& chunk_pos ) {
	typedef std::map<std::shared_ptr<const sf::Texture>, GLuint> TextureVBOMap;

	sf::Clock clock;

	flex::Chunk::Vector block_runner;
	flex::Chunk::Vector chunk_size = m_planet.get_chunk_size();
	const flex::Class* block_cls = nullptr;
	sf::Vector3f offset(
		static_cast<float>( chunk_pos.x * m_planet.get_chunk_size().x ) * GRID_SIZE,
		static_cast<float>( chunk_pos.y * m_planet.get_chunk_size().y ) * GRID_SIZE,
		static_cast<float>( chunk_pos.z * m_planet.get_chunk_size().z ) * GRID_SIZE
	);
	TextureVBOMap vbos;

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
					}

					// Get VBO or generate a new one.

					// Iterate over triangles.
					for( flex::Mesh::TriangleIndex tri_idx = 0; tri_idx < mesh.get_num_triangles(); ++tri_idx ) {
						const flex::Triangle& tri = mesh.get_triangle( tri_idx );

					}
				}

				offset.x += GRID_SIZE;
			}

			offset.y += GRID_SIZE;
		}

		offset.z += GRID_SIZE;
	}

	std::cout << clock.GetElapsedTime().AsMicroseconds() << " µs" << std::endl;
}

void PlanetRenderer::render() const {
}
