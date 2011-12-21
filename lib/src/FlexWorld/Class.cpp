#include <FlexWorld/Class.hpp>

#include <cassert>

namespace flex {

Class::Class( const FlexID& id ) :
	m_id( id )
{
	assert( m_id.is_valid_resource() );
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

}
