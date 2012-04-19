#include "EntityGroupNode.hpp"
#include "ClassDrawable.hpp"

#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

#include <iostream> // XXX 

EntityGroupNode::Ptr EntityGroupNode::create( ResourceManager& resource_manager, sg::Renderer& renderer ) {
	Ptr ptr( new EntityGroupNode( resource_manager, renderer ) );
	return ptr;
}

EntityGroupNode::EntityGroupNode( ResourceManager& resource_manager, sg::Renderer& renderer ) :
	sg::Node(),
	m_resource_manager( resource_manager ),
	m_renderer( renderer )
{
}

void EntityGroupNode::add_entity( const flex::Entity& entity ) {
	assert( m_class_drawables.find( entity.get_id() ) == m_class_drawables.end() );

	// Create the drawable for the entity (class).
	ClassDrawable::Ptr drw = ClassDrawable::create( m_renderer, m_resource_manager );
	drw->set_class( entity.get_class() );

	m_class_drawables[entity.get_id()] = drw;

	// Attach to myself.
	attach( drw );

	std::cout << "Entity added: " << entity.get_id() << std::endl;

	// Update entity to set initial stuff.
	update_entity( entity );
}

void EntityGroupNode::update_entity( const flex::Entity& entity ) {
	EntityClassDrawableMap::iterator iter = m_class_drawables.find( entity.get_id() );
	assert( iter != m_class_drawables.end() );

	iter->second->set_local_transform(
		sg::Transform(
			entity.get_position() - entity.get_class().get_origin(),
			entity.get_rotation(),
			entity.get_class().get_scale(),
			entity.get_class().get_origin()
		)
	);

	std::cout << "Entity updated: " << entity.get_id() << std::endl;
}

void EntityGroupNode::remove_entity( const flex::Entity& entity ) {
	std::cout << "Entity removed: " << entity.get_id() << std::endl;
}
