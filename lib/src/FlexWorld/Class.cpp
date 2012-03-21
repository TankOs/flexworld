#include <FlexWorld/Class.hpp>

#include <cassert>

namespace flex {

Class::Class( const FlexID& id ) :
	m_id( id ),
	m_model( nullptr ),
	m_name( "" ),
	m_origin( 0, 0, 0 ),
	m_scale( 1, 1, 1 )
{
	assert( m_id.is_valid_resource() );
}

Class::Class( const Class& other ) {
	*this = other;
}

const Class& Class::operator=( const Class& other ) {
	m_hooks = other.m_hooks;
	m_textures = other.m_textures;
	m_id = other.m_id;
	m_name = other.m_name;
	m_origin = other.m_origin;
	m_scale = other.m_scale;

	if( other.m_model ) {
		m_model.reset( new Resource( *other.m_model ) );
	}
	else {
		m_model.reset();
	}

	return *this;
}

const FlexID& Class::get_id() const {
	return m_id;
}

void Class::set_name( const std::string& name ) {
	m_name = name;
}

const std::string& Class::get_name() const {
	return m_name;
}

void Class::add_texture( const Resource& texture ) {
	m_textures.push_back( texture );
}

const Resource& Class::get_texture( std::size_t index ) const {
	assert( has_texture( index ) );
	return m_textures[index];
}

void Class::set_hook( const std::string& id, const sf::Vector3f& position ) {
	assert( !id.empty() );
	m_hooks[id] = position;
}

const sf::Vector3f* Class::find_hook( const std::string& id ) const {
	HookMap::const_iterator iter( m_hooks.find( id ) );
	return iter == m_hooks.end() ? nullptr : &iter->second;
}

void Class::set_origin( const sf::Vector3f& origin ) {
	m_origin = origin;
}

const sf::Vector3f& Class::get_origin() const {
	return m_origin;
}

std::size_t Class::get_num_textures() const {
	return m_textures.size();
}

std::size_t Class::get_num_hooks() const {
	return m_hooks.size();
}

bool Class::has_texture( std::size_t index ) const {
	return index < m_textures.size();
}

bool Class::has_model() const {
	return m_model.get() != nullptr;
}

const Resource& Class::get_model() const {
	return *m_model;
}

void Class::set_model( const Resource& model ) {
	m_model.reset( new Resource( model ) );
}

void Class::set_scale( const sf::Vector3f& scale ) {
	m_scale = scale;
}

const sf::Vector3f& Class::get_scale() const {
	return m_scale;
}

}
