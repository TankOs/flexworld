#pragma once

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

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
		Planet* find_planet( const std::string& id );

		/** Create new planet.
		 * Giving an already used planet ID leads to serious problems. Use
		 * find_planet() to check before!
		 * @param id ID.
		 * @param size Size (in chunks).
		 * @param chunk_size Chunk size (in blocks).
		 */
		void create_planet( const std::string& id, const Planet::Vector& size, const Chunk::Vector& chunk_size );

		/** Get number of classes.
		 * @return Number of classes.
		 */
		std::size_t get_num_classes() const;

		/** Find class by ID.
		 * @param id ID.
		 * @return Class or nullptr.
		 */
		const Class* find_class( const FlexID& id ) const;

		/** Add a class.
		 * The given class is copied. The classes ID must be unique, otherwise
		 * undefined behaviour.
		 * @param cls Class.
		 */
		void add_class( const Class& cls );

	private:
		typedef std::map<const std::string, Planet*> PlanetMap;
		typedef std::map<const std::string, Entity> EntityMap;
		typedef std::map<const std::string, Class> ClassMap;

		void wipe();

		PlanetMap m_planets;
		EntityMap m_entities;
		ClassMap m_classes;
};

}
