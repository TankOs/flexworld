#include <FlexWorld/Class.hpp>

#include <cassert>
#include <limits>

namespace flex {

const sf::Vector3f Class::INVISIBLE_HOOK = sf::Vector3f(
	std::numeric_limits<float>::max(),
	std::numeric_limits<float>::max(),
	std::numeric_limits<float>::max()
);

Class::Class( const FlexID& id ) :
	m_name( "" ),
	m_model( nullptr ),
	m_bounding_box( 0, 0, 0, 1, 1, 1 ),
	m_origin( 0, 0, 0 ),
	m_scale( 1, 1, 1 ),
	m_id( id )
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
	m_bounding_box = other.m_bounding_box;

	if( other.m_model ) {
		m_model.reset( new Resource( *other.m_model ) );
	}
	else {
		m_model.reset();
	}

	if( other.m_container_image ) {
		m_container_image.reset( new Resource( *other.m_container_image ) );
	}
	else {
		m_container_image.reset();
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

void Class::set_bounding_box( const FloatCuboid& cuboid ) {
	assert( cuboid.width > 0 );
	assert( cuboid.height > 0 );
	assert( cuboid.depth > 0 );

	m_bounding_box = cuboid;
}

const FloatCuboid& Class::get_bounding_box() const {
	return m_bounding_box;
}

bool Class::has_container_image() const {
	return m_container_image.get() != nullptr;
}

void Class::set_container_image( const Resource& image ) {
	m_container_image.reset( new Resource( image ) );
}

const Resource& Class::get_container_image() const {
	assert( m_container_image );
	return *m_container_image;
}


}
