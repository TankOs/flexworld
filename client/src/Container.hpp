#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/String.hpp>
#include <memory>
#include <map>
#include <cstdint>

namespace sf {
class RenderTarget;
class Event;
}

/** Visual container.
 */
class Container {
	public:
		/** Ctor.
		 * @param id ID.
		 */
		Container( uint32_t id = 0 );

		/** Get ID.
		 * @return ID.
		 */
		uint32_t get_id() const;

		/** Set background texture.
		 * @param texture Texture.
		 */
		void set_background_texture( std::shared_ptr<const sf::Texture> texture );

		/** Update entity.
		 * If the entity doesn't exist it's created.
		 * @param id ID.
		 * @param name Name.
		 * @param amount Amount.
		 * @param texture Texture.
		 */
		void update_entity(
			uint32_t id,
			const sf::String& name,
			uint32_t amount,
			std::shared_ptr<const sf::Texture> texture
		);

		/** Render.
		 * @param target Render target.
		 */
		void render( sf::RenderTarget& target ) const;

		/** Handle SFML event.
		 * @param event Event.
		 */
		void handle_event( const sf::Event& event );

		/** Get position.
		 * @return Position.
		 */
		const sf::Vector2f& get_position() const;

		/** Set position.
		 * @param position Position.
		 */
		void set_position( const sf::Vector2f& position );

		/** Get size.
		 * @return Size.
		 */
		sf::Vector2f get_size() const;

	private:
		typedef std::shared_ptr<const sf::Texture> TexturePtrConst;

		struct EntityInfo {
			sf::String name;
			uint32_t amount;

			TexturePtrConst texture;
			sf::Sprite sprite;
		};

		typedef std::map<uint32_t, EntityInfo> EntityInfoMap;


		EntityInfoMap m_entities;

		sf::Sprite m_background_sprite;

		TexturePtrConst m_background_texture;

		uint32_t m_id;
};
