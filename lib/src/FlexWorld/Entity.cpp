#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>

#include <cassert>

namespace flex {

Entity::Entity( const Class& cls ) :
	m_position( 0, 0, 0 ),
	m_id( 0 ),
	m_amount( 1 ),
	m_class( &cls )
{
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
	m_name.reset( new std::string( name ) );
}

void Entity::reset_name() {
	m_name.reset();
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

}
