#include <FlexWorld/World.hpp>

#include <cassert>

namespace fw {

World::World() :
	m_next_entity_id( 0 )
{
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

Planet* World::find_planet( const std::string& id ) {
	return const_cast<Planet*>( static_cast<const World*>( this )->find_planet( id ) );
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

	EntityMap::iterator ent_iter( m_entities.begin() );
	EntityMap::iterator ent_iter_end( m_entities.end() );
	
	for( ; ent_iter != ent_iter_end; ++ent_iter ) {
		delete ent_iter->second;
	}

	m_planets.clear();
	m_entities.clear();
	m_links.clear();
	m_classes.clear();
}

std::size_t World::get_num_classes() const {
	return m_classes.size();
}

const Class* World::find_class( const FlexID& id ) const {
	assert( id.is_valid_resource() );

	ClassMap::const_iterator c_iter( m_classes.find( id.get() ) );
	return c_iter == m_classes.end() ? nullptr : &c_iter->second;
}

void World::add_class( const Class& cls ) {
	assert( m_classes.find( cls.get_id().get() ) == m_classes.end() );

	m_classes.insert( std::pair<const std::string, Class>( cls.get_id().get(), cls ) );
}

Entity& World::create_entity( const FlexID& class_id ) {
	assert( class_id.is_valid_resource() );

	ClassMap::iterator cls_iter = m_classes.find( class_id.get() );
	assert( cls_iter != m_classes.end() );

	Entity* ent = new Entity( cls_iter->second );
	ent->set_id( m_next_entity_id++ );

	std::pair<EntityMap::iterator, bool> result = m_entities.insert(
		std::pair<const Entity::ID, Entity*>(
			ent->get_id(),
			ent
		)
	);

	return *result.first->second;
}

Entity* World::find_entity( Entity::ID id ) {
	if( id >= m_next_entity_id ) {
		return nullptr;
	}

	EntityMap::iterator ent_iter = m_entities.find( id );

	if( ent_iter == m_entities.end() ) {
		return nullptr;
	}

	return ent_iter->second;
}

const Entity* World::find_entity( Entity::ID id ) const {
	if( id >= m_next_entity_id ) {
		return nullptr;
	}

	EntityMap::const_iterator ent_iter = m_entities.find( id );

	if( ent_iter == m_entities.end() ) {
		return nullptr;
	}

	return ent_iter->second;
}

void World::delete_entity( Entity::ID id ) {
	assert( find_entity( id ) != nullptr );

	m_entities.erase( id );
}

void World::link_entity_to_planet( Entity::ID entity_id, const std::string& planet_id ) {
	Entity* ent = find_entity( entity_id );
	Planet* planet = find_planet( planet_id );

	assert( ent );
	assert( ent->get_parent() == nullptr );
	assert( planet );

	// Get previous link (if any).
	LinkMap::iterator link_iter = m_links.find( entity_id );

	// Remove link from planet.
	if( link_iter != m_links.end() ) {
		assert( link_iter->second->has_entity( *ent ) );
		link_iter->second->remove_entity( *ent );
	}

	m_links[entity_id] = planet;

	// Add to planet.
	planet->add_entity( *ent );
}

Planet* World::find_linked_planet( Entity::ID entity_id ) {
	assert( find_entity( entity_id ) != nullptr );

	LinkMap::iterator link_iter = m_links.find( entity_id );
	return link_iter != m_links.end() ? link_iter->second : nullptr;
}

void World::unlink_entity_from_planet( Entity::ID entity_id ) {
	assert( find_linked_planet( entity_id ) != nullptr );

	Entity* ent = find_entity( entity_id );
	assert( ent != nullptr );
	assert( ent->get_parent() == nullptr );

	LinkMap::iterator link_iter = m_links.find( entity_id );

	if( link_iter != m_links.end() ) {
		link_iter->second->remove_entity( *ent );
		m_links.erase( entity_id );
	}
}

World::PlanetConstIterator World::planets_begin() const {
	return m_planets.begin();
}

World::PlanetConstIterator World::planets_end() const {
	return m_planets.end();
}

void World::attach_entity( Entity::ID source_id, Entity::ID target_id, const std::string& hook_id ) {
	Entity* source = find_entity( source_id );
	Entity* target = find_entity( target_id );

	assert( source != nullptr );
	assert( target != nullptr );

	// Create hook at target entity if it doesn't exist.
	if( target->get_class().find_hook( hook_id ) == nullptr ) {
		ClassMap::iterator cls = m_classes.find( target->get_class().get_id().get() );
		assert( cls != m_classes.end() );

		cls->second.set_hook( hook_id, sf::Vector3f( Class::INVISIBLE_HOOK ) );
	}

	// Detach entity if already attached.
	if( source->get_parent() != nullptr ) {
		Entity* parent_entity = find_entity( source->get_parent()->get_id() );
		assert( parent_entity );

		parent_entity->detach( *source );
	}

	// Find source entity's planet.
	Planet* source_planet = find_linked_planet( source_id );

	// Remove and unlink from current planet (if any).
	if( source_planet ) {
		unlink_entity_from_planet( source_id );
	}

	// Attach.
	target->attach( *source, hook_id );

	// Reset position.
	source->set_position( sf::Vector3f( 0, 0, 0 ) );
}

void World::detach_entity( Entity::ID entity_id ) {
	Entity* entity = find_entity( entity_id );

	assert( entity != nullptr );
	assert( entity->get_parent() != nullptr );

	// Detach from parent.
	Entity* parent = find_entity( entity->get_parent()->get_id() );

	assert( parent != nullptr );

	parent->detach( *entity );

	// Reset entity's position + rotation.
	entity->set_position( parent->get_position() );
	entity->set_rotation( parent->get_rotation() );

	// Link entity to parent's planet (if any).
	Planet* parent_planet = find_linked_planet( parent->get_id() );

	if( parent_planet ) {
		link_entity_to_planet( entity->get_id(), parent_planet->get_id() );
	}
}

}
