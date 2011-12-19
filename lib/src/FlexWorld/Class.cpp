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

const Resource& Class::get_texture( std::size_t index ) const {
	if( index >= m_textures.size() ) {
		throw std::invalid_argument( "Invalid texture index." );
	}

	return m_textures[index];
}

void Class::set_hook( const std::string& id, const sf::Vector3f& position ) {
	m_hooks[id] = position;
}

const sf::Vector3f& Class::get_hook( const std::string& id ) const {
	HookMap::const_iterator iter( m_hooks.find( id ) );

	if( iter == m_hooks.end() ) {
		throw std::invalid_argument( "Invalid hook ID." );
	}

	return iter->second;
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

}
