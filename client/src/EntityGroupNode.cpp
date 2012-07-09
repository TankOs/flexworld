#include "EntityGroupNode.hpp"
#include "ClassDrawable.hpp"

#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

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

void EntityGroupNode::add_entity( const fw::Entity& entity ) {
	assert( m_class_drawables.find( entity.get_id() ) == m_class_drawables.end() );

	// Traverse up the entity's hierarchy.
	const fw::Entity* parent = entity.get_parent();
	ClassDrawable::Ptr parent_node;

	if( parent != nullptr ) {
		parent_node = m_class_drawables[parent->get_id()];
		assert( parent_node );
	}

	// Create the drawable for the entity (class).
	ClassDrawable::Ptr drw = ClassDrawable::create( m_renderer, m_resource_manager );
	drw->set_class( entity.get_class() );

	// Either attach to parent node or create new root node.
	m_class_drawables[entity.get_id()] = drw;

	if( !parent_node ) {
		attach( drw );
	}
	else {
		parent_node->attach( drw );
	}

	// Update entity to set initial stuff.
	update_entity( entity );
}

void EntityGroupNode::update_entity( const fw::Entity& entity ) {
	EntityClassDrawableMap::iterator iter = m_class_drawables.find( entity.get_id() );
	assert( iter != m_class_drawables.end() );

	// Calculate transform including hook offset.
	sf::Vector3f position = entity.get_position();
	const fw::Entity* parent = entity.get_parent();

	if( parent != nullptr ) {
		// Find out hook the entity is attached to.
		const std::string& hook = parent->get_child_hook( entity );

		position += *parent->get_class().find_hook( hook );
	}

	iter->second->set_local_transform( 
		sg::Transform(
			position,
			entity.get_rotation(),
			entity.get_class().get_scale(),
			entity.get_class().get_origin()
		)
	);
}

void EntityGroupNode::remove_entity( const fw::Entity& /*entity*/ ) {
}
