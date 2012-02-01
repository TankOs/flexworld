#pragma once

#include <FlexWorld/FlexID.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <map>
#include <memory>

/** Resource manager.
 * Manages textures and models.
 */
class ResourceManager {
	public:
		/** Set base path.
		 * @param path Path.
		 */
		void set_base_path( const std::string& path );

		/** Load texture from file.
		 * A texture with the same ID gets removed (even if loading fails).
		 * @param id ID.
		 * @return true on success.
		 */
		bool load_texture( const flex::FlexID& id );

		/** Find texture.
		 * @param id ID.
		 * @return Texture or nullptr if not found.
		 */
		std::shared_ptr<const sf::Texture> find_texture( const flex::FlexID& id ) const;

		/** Garbage collect.
		 * All resources with a use count of 1 are unloaded.
		 */
		void garbage_collect();

	private:
		typedef std::map<const std::string, std::shared_ptr<sf::Texture>> TextureMap;

		TextureMap m_textures;

		std::string m_base_path;
};
