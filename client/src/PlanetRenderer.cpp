#include "PlanetRenderer.hpp"

#include <cassert>
#include <iostream> // XXX 

PlanetRenderer::PlanetRenderer() :
	m_planet( nullptr )
{
}

void PlanetRenderer::set_planet( const flex::Planet& planet ) {
	m_planet = &planet;
}

void PlanetRenderer::prepare_chunk( const flex::Planet::Vector& chunk_pos ) {
	assert( m_planet );

	if( !m_planet ) {
		return;
	}

	std::cout << "Preparing chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
}
