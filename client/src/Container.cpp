#include "Container.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>
#include <cassert>

Container::Container( uint32_t id ) :
	m_id( id )
{
}

void Container::set_background_texture( TexturePtrConst texture ) {
	m_background_texture = texture;
	m_background_sprite.setTexture( *texture, true );
}

uint32_t Container::get_id() const {
	return m_id;
}

void Container::update_entity(
	uint32_t id,
	const sf::String& name,
	uint32_t amount,
	std::shared_ptr<const sf::Texture> texture
) {
	assert( amount > 0 );
	assert( texture );

	EntityInfo& info = m_entities[id];

	info.name = name;
	info.amount = amount;
	info.texture = texture;
	info.sprite = sf::Sprite( *texture );
}

void Container::render( sf::RenderTarget& target ) const {
	target.draw( m_background_sprite );
}

void Container::handle_event( const sf::Event& /*event*/ ) {
}

const sf::Vector2f& Container::get_position() const {
	return m_background_sprite.getPosition();
}

void Container::set_position( const sf::Vector2f& position ) {
	m_background_sprite.setPosition( position );
}

sf::Vector2f Container::get_size() const {
	return sf::Vector2f(
		m_background_sprite.getGlobalBounds().width,
		m_background_sprite.getGlobalBounds().height
	);
}
