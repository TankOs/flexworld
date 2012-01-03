#include <FlexWorld/World.hpp>

#include <cassert>

namespace flex {

World::World() {
}

World::~World() {
	wipe();
}

std::size_t World::get_num_planets() const {
	return m_planets.size();
}

std::size_t World::get_num_entities() const {
	return m_entities.size();
}

const Planet* World::find_planet( const std::string& id ) const {
	assert( !id.empty() );

	PlanetMap::const_iterator iter( m_planets.find( id ) );
	return iter != m_planets.end() ? iter->second : nullptr;
}

void World::create_planet( const std::string& id, const Planet::Vector& size, const Chunk::Vector& chunk_size ) {
	assert( !id.empty() );
	assert( size.x > 0 && size.y > 0 && size.z > 0 );
	assert( chunk_size.x > 0 && chunk_size.y > 0 && chunk_size.z > 0 );
	assert( find_planet( id ) == nullptr );

	m_planets[id] = new Planet( id, size, chunk_size );
}

void World::wipe() {
	PlanetMap::iterator planet_iter( m_planets.begin() );
	PlanetMap::iterator planet_iter_end( m_planets.end() );
	
	for( ; planet_iter != planet_iter_end; ++planet_iter ) {
		delete planet_iter->second;
	}

	m_entities.clear();
	m_planets.clear();
}

}
