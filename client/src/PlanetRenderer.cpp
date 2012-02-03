#include "PlanetRenderer.hpp"

#include <SFML/System/Clock.hpp>
#include <cassert>
#include <iostream> // XXX 

PlanetRenderer::PlanetRenderer( const flex::Planet& planet, const ResourceManager& resource_manager ) :
	m_planet( planet ),
	m_resource_manager( resource_manager )
{
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

			}
		}
	}

	//std::cout << clock.GetElapsedTime().AsMicroseconds() << " µs" << std::endl;
}

void PlanetRenderer::render() const {
}
