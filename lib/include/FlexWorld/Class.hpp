#pragma once

#include <FlexWorld/Resource.hpp>

#include <string>
#include <map>
#include <vector>
#include <SFML/System/Vector3.hpp>

namespace flex {

/** FlexWorld class.
 * A class describes properties for objects (blocks and entities) in FlexWorld.
 */
class Class {
	public:
		/** Ctor.
		 * @param id ID.
		 */
		Class( const ResourceId& id );

		/** Get ID.
		 * @return ID.
		 */
		const ResourceId& get_id() const;

		/** Set name.
		 * @param name Name.
		 */
		void set_name( const std::string& name );

		/** Get name.
		 * @return Name.
		 */
		const std::string& get_name() const;

		/** Set origin.
		 * @param origin Origin.
		 */
		void set_origin( const sf::Vector3f& origin );

		/** Get origin.
		 * @return Origin.
		 */
		const sf::Vector3f& get_origin() const;

		/** Add texture.
		 * @param texture Texture (copied).
		 */
		void add_texture( const Resource& texture );

		/** Get texture.
		 * @param index Index.
		 * @return Texture resource.
		 * @throws std::invalid_argument if index is invalid.
		 */
		const Resource& get_texture( std::size_t index ) const;

		/** Get number of textures.
		 * @return Number of textures.
		 */
		std::size_t get_num_textures() const;

		/** Set hook.
		 * @param id ID.
		 * @param position Position.
		 */
		void set_hook( const std::string& id, const sf::Vector3f& position );

		/** Get hook.
		 * @param id ID.
		 * @return Hook.
		 * @throws std::invalid_argument if ID is invalid.
		 */
		const sf::Vector3f& get_hook( const std::string& id ) const;

		/** Get number of hooks.
		 * @return Number of hooks.
		 */
		std::size_t get_num_hooks() const;

	private:
		typedef std::map<const std::string, sf::Vector3f> HookMap;
		typedef std::vector<Resource> ResourceVector;

		ResourceId m_id;
		HookMap m_hooks;
		ResourceVector m_textures;
		sf::Vector3f m_origin;
		std::string m_name;
};

}
