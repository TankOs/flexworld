#include <FlexWorld/Class.hpp>

namespace flex {

Class::Class( const ResourceId& id ) :
	m_id( id )
{
}

const ResourceId& Class::get_id() const {
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

const Resource* Class::get_texture( std::size_t index ) const {
	if( index >= m_textures.size() ) {
		return nullptr;
	}

	return &m_textures[index];
}

void Class::set_hook( const std::string& id, const sf::Vector3f& position ) {
	m_hooks[id] = position;
}

const sf::Vector3f* Class::get_hook( const std::string& id ) const {
	HookMap::const_iterator iter( m_hooks.find( id ) );

	return iter == m_hooks.end() ? nullptr : &iter->second;
}

void Class::set_origin( const sf::Vector3f& origin ) {
	m_origin = origin;
}

const sf::Vector3f& Class::get_origin() const {
	return m_origin;
}

}
