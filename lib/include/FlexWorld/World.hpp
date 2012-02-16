#pragma once

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

#include <map>

namespace flex {

/** World.
 * The world holds planets and entities and is used as a global storage.
 *
 * Please use this classes methods over children's methods if applicable. For
 * example to add an entity to a planet, use link_entity_to_planet() instead of
 * calling Planet::add_entity() yourself.
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

		/** Create entity.
		 * @param class_id Class ID (must exist, otherwise undefined behaviour).
		 * @return Newly created entity, will be managed by world.
		 */
		Entity& create_entity( const FlexID& class_id );

		/** Find entity by ID.
		 * @param id ID.
		 * @return Entity or nullptr if not found.
		 */
		Entity* find_entity( Entity::ID id );

		/** Delete entity.
		 * @param id ID (must be valid).
		 */
		void delete_entity( Entity::ID id );

		/** Create link between entity and planet.
		 * If the entity has been linked before, that link will be overwritten.
		 * @param entity_id Entity ID (must exist).
		 * @param planet_id Planet ID (must exist).
		 */
		void link_entity_to_planet( Entity::ID entity_id, const std::string& planet_id );

		/** Find an entity's linked planet.
		 * @param entity_id Entity ID (must exist).
		 * @return Linked planet or nullptr if not linked.
		 */
		Planet* find_linked_planet( Entity::ID entity_id );

		/** Unlink entity from planet.
		 * Undefined behaviour if link doesn't exist.
		 * @param entity_id Entity ID (must exist).
		 */
		void unlink_entity_from_planet( Entity::ID entity_id );

	private:
		typedef std::map<const std::string, Planet*> PlanetMap;
		typedef std::map<const Entity::ID, Entity*> EntityMap;
		typedef std::map<const Entity::ID, Planet*> LinkMap;
		typedef std::map<const std::string, Class> ClassMap;

		void wipe();

		PlanetMap m_planets;
		EntityMap m_entities;
		ClassMap m_classes;
		LinkMap m_links;

		Entity::ID m_next_entity_id;
};

}
