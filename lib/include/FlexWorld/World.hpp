#pragma once

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Entity.hpp>

#include <map>

namespace flex {

/** World.
 * The world holds planets and entities and is used as a global storage.
 */
class World {
	public:
		/** Ctor.
		 */
		World();

		/** Dtor.
		 */
		~World();

		/** Get number of planets.
		 * @return Number of planets.
		 */
		std::size_t get_num_planets() const;

		/** Get number of entities.
		 * @return Number of entities.
		 */
		std::size_t get_num_entities() const;

		/** Find planet.
		 * @param id ID.
		 * @return Planet or nullptr.
		 */
		const Planet* find_planet( const std::string& id ) const;

		/** Create new planet.
		 * Giving an already used planet ID leads to serious problems. Use
		 * find_planet() to check before!
		 * @param id ID.
		 * @param size Size (in chunks).
		 * @param chunk_size Chunk size (in blocks).
		 */
		void create_planet( const std::string& id, const Planet::Vector& size, const Chunk::Vector& chunk_size );

	private:
		typedef std::map<const std::string, Planet*> PlanetMap;
		typedef std::map<const std::string, Entity> EntityMap;

		void wipe();

		PlanetMap m_planets;
		EntityMap m_entities;
};

}
