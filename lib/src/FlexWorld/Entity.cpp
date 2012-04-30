#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

#include <algorithm>
#include <iostream>
#include <cassert>

namespace flex {

Entity::Entity( const Class& cls ) :
	m_position( 0, 0, 0 ),
	m_rotation( 0, 0, 0 ),
	m_id( 0 ),
	m_amount( 1 ),
	m_children( nullptr ),
	m_name( nullptr ),
	m_class( &cls ),
	m_parent( nullptr )
{
}

/*Entity::Entity( const Entity& other ) :
	m_position( other.m_position ),
	m_rotation( other.m_rotation ),
	m_id( other.m_id ),
	m_amount( other.m_amount ),
	m_class( other.m_class ),
	m_parent( nullptr )
{
	delete m_name;
	m_name = nullptr;

	if( other.m_name ) {
		m_name = new std::string( *other.m_name );
	}

#if !defined( NDEBUG )
	if( other.m_parent != nullptr ) {
		std::cout << "WARNING: Copied entity in attached state." << std::endl;
	}
#endif
}*/

Entity::~Entity() {
	delete m_children;
	delete m_name;
}

Entity::ID Entity::get_id() const {
	return m_id;
}

void Entity::set_id( ID id ) {
	m_id = id;
}

const Class& Entity::get_class() const {
	return *m_class;
}

const std::string& Entity::get_name() const {
	return m_name ? *m_name : m_class->get_name();
}

void Entity::set_name( const std::string& name ) {
	delete m_name;
	m_name = new std::string( name );
}

void Entity::reset_name() {
	delete m_name;
	m_name = nullptr;
}

Entity::AmountType Entity::get_amount() const {
	return m_amount;
}

void Entity::set_amount( AmountType amount ) {
	assert( amount > 0 );
	m_amount = amount;
}

const sf::Vector3f& Entity::get_position() const {
	return m_position;
}

void Entity::set_position( const sf::Vector3f& position ) {
	m_position = position;
}

/*Entity& Entity::operator=( const Entity& other ) {
	delete m_name;
	m_name = nullptr;

	m_position = other.m_position;
	m_rotation = other.m_rotation;
	m_id = other.m_id;
	m_amount = other.m_amount;
	m_class = other.m_class;
	m_parent = nullptr;

#if !defined( NDEBUG )
	if( other.m_parent != nullptr ) {
		std::cout << "WARNING: Copied entity in attached state." << std::endl;
	}
#endif

	if( other.m_name ) {
		m_name = new std::string( *other.m_name );
	}

	return *this;
}*/

void Entity::set_rotation( const sf::Vector3f& rotation ) {
	m_rotation = rotation;
}

const sf::Vector3f& Entity::get_rotation() const {
	return m_rotation;
}

const Entity* Entity::get_parent() const {
	return m_parent;
}

void Entity::set_parent( const Entity* parent ) {
	m_parent = parent;
}

void Entity::attach( Entity& child, const std::string& hook_id ) {
	assert( child.get_parent() == nullptr );
	assert( m_class->find_hook( hook_id ) != nullptr );

	if( !m_children ) {
		m_children = new HookEntityMap;
	}

	(*m_children)[hook_id].push_back( &child );
	child.set_parent( this );
}

void Entity::detach( Entity& child ) {
	assert( child.get_parent() == this );
	assert( m_children );

#if !defined( NDEBUG )
	bool found = false;
#endif

	HookEntityMap::iterator hook_iter( m_children->begin() );
	HookEntityMap::iterator hook_iter_end( m_children->end() );
	
	for( ; hook_iter != hook_iter_end; ++hook_iter ) {
		EntityPtrArray& ents = hook_iter->second;

		EntityPtrArray::iterator ent_iter = std::find( ents.begin(), ents.end(), &child );

		if( ent_iter != ents.end() ) {
			ents.erase( ent_iter );
			child.set_parent( nullptr );

			// Check if no children left for hook.
			if( ents.size() == 0 ) {
				m_children->erase( hook_iter );
			}

#if !defined( NDEBUG )
			found = true;
#endif

			break;
		}
	}

	assert( found == true );

	// If no hook has an attached children, remove container.
	if( m_children->size() == 0 ) {
		delete m_children;
		m_children = nullptr;
	}
}

std::size_t Entity::get_num_children() const {
	if( !m_children ) {
		return 0;
	}

	std::size_t num = 0;

	HookEntityMap::const_iterator hook_iter = m_children->begin();
	HookEntityMap::const_iterator hook_iter_end = m_children->end();
	
	for( ; hook_iter != hook_iter_end; ++hook_iter ) {
		num += hook_iter->second.size();
	}

	return num;
}

std::size_t Entity::get_num_children( const std::string& hook_id ) const {
	assert( m_class->find_hook( hook_id ) != nullptr );

	if( !m_children ) {
		return 0;
	}

	HookEntityMap::const_iterator hook_iter = m_children->find( hook_id );

	return (hook_iter != m_children->end()) ? hook_iter->second.size() : 0;
}

bool Entity::has_child( const Entity& child ) const {
	return m_children && child.get_parent() == this;
}

bool Entity::has_child( const Entity& child, const std::string& hook_id ) const {
	assert( m_class->find_hook( hook_id ) != nullptr );

	if( !m_children || child.get_parent() != this ) {
		return false;
	}

	HookEntityMap::const_iterator hook_iter = m_children->find( hook_id );

	if( hook_iter == m_children->end() ) {
		return false;
	}

	return std::find( hook_iter->second.begin(), hook_iter->second.end(), &child ) != hook_iter->second.end();
}

Entity& Entity::get_child( const std::string& hook_id, std::size_t index ) const {
	assert( m_class->find_hook( hook_id ) != nullptr );
	assert( m_children );

	HookEntityMap::const_iterator hook_iter = m_children->find( hook_id );
	assert( hook_iter != m_children->end() );
	assert( index < hook_iter->second.size() );

	return *hook_iter->second[index];
}

}
