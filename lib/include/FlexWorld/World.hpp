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
 *
 * Attached entities are automagically managed by World, that means you're not
 * allowed to call link_entity_to_planet() and unlink_entity_from_planet() for
 * them. Make sure to use the World's attach/detach functions instead of the
 * Entity's to ensure valid data. This is required to drop attached entities
 * from the planet octrees. Attached entities are also not linked to planets.
 * One always has to check the link of the uppermost parent of the attached
 * entity.
 */
class World {
	public:
		typedef std::map<const std::string, Planet*>::const_iterator PlanetConstIterator; ///< Planet iterator (const).

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

		/** Find entity by ID.
		 * @param id ID.
		 * @return Entity or nullptr if not found.
		 */
		const Entity* find_entity( Entity::ID id ) const;

		/** Delete entity.
		 * @param id ID (must be valid).
		 */
		void delete_entity( Entity::ID id );

		/** Create link between entity and planet.
		 * If the entity has been linked before, that link will be overwritten.
		 * Entity must be in a detached state.
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
		 * Undefined behaviour if link doesn't exist or entity is in an attached
		 * state.
		 * @param entity_id Entity ID (must exist).
		 */
		void unlink_entity_from_planet( Entity::ID entity_id );

		/** Attach an entity to another entity.
		 * Undefined behaviour if one of the entities weren't added or hook is
		 * invalid. It's perfectly sane to reattach entities to another parent.
		 * Attached entities lose their planet link. Position is reset to (0; 0;
		 * 0).
		 * @param source_id ID of entity to attach.
		 * @param target_id ID of entity to attach to.
		 * @param hook_id ID of hook.
		 */
		void attach_entity( Entity::ID source_id, Entity::ID target_id, const std::string& hook_id );

		/** Detach an entity.
		 * After detaching the entity will be positioned at the exact parent's
		 * location (same goes for rotation). Also a link to the paren't planet (if
		 * any) will be created.
		 *
		 * Undefined behaviour if entity is not attached.
		 * @param entity_id ID of entity to detach from parent.
		 */
		void detach_entity( Entity::ID entity_id );

		/** Get iterator to first planet.
		 * If there's no planet, this iterator is equal to planets_end().
		 * @return Iterator.
		 */
		PlanetConstIterator planets_begin() const;

		/** Get iterator to field behind last planet.
		 * @return Iterator.
		 */
		PlanetConstIterator planets_end() const;

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
