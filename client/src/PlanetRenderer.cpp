#include <GL/glew.h>

#include "PlanetRenderer.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Model.hpp>
#include <FlexWorld/Class.hpp>

#include <SFML/System/Clock.hpp>
#include <cassert>
#include <iostream> // XXX 

PlanetRenderer::PlanetRenderer( const flex::Planet& planet, const ResourceManager& resource_manager ) :
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
	sf::Clock clock;

	flex::Chunk::Vector block_runner;
	flex::Chunk::Vector chunk_size = m_planet.get_chunk_size();
	const flex::Class* block_cls = nullptr;

	for( block_runner.z = 0; block_runner.z < chunk_size.z; ++block_runner.z ) {
		for( block_runner.y = 0; block_runner.y < chunk_size.y; ++block_runner.y ) {
			for( block_runner.x = 0; block_runner.x < chunk_size.x; ++block_runner.x ) {
				// Skip empty blocks.
				block_cls = m_planet.find_block( chunk_pos, block_runner );

				if( block_cls == nullptr ) {
					continue;
				}

				// Get model.
				std::shared_ptr<const flex::Model> model = m_resource_manager.find_model( block_cls->get_model().get_id() );
				assert( model );

				// Iterate over meshes.
				for( std::size_t mesh_idx = 0; mesh_idx < model->get_num_meshes(); ++mesh_idx ) {
					const flex::Mesh& mesh = model->get_mesh( mesh_idx );

					// Get texture.
					// FIXME: Possible multi-threading problem with resource manager.
					//std::shared_ptr<const sf::Texture> texture = m_resource_manager.find_texture( 

					// Iterate over triangles.
					for( flex::Mesh::TriangleIndex tri_idx = 0; tri_idx < mesh.get_num_triangles(); ++tri_idx ) {
						const flex::Triangle& tri = mesh.get_triangle( tri_idx );

					}
				}
			}
		}
	}

	std::cout << clock.GetElapsedTime().AsMicroseconds() << " µs" << std::endl;
}

void PlanetRenderer::render() const {
}
