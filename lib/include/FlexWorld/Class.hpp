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
		Class( const FlexId& id );

		/** Get ID.
		 * @return ID.
		 */
		const FlexId& get_id() const;

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
		 * @param texture Texture.
		 */
		void add_texture( const Resource& texture );

		/** Get texture.
		 * @param index Index.
		 * @return Resource or null if not found.
		 */
		const Resource* get_texture( std::size_t index ) const;

		/** Set hook.
		 * @param id ID.
		 * @param position Position.
		 */
		void set_hook( const std::string& id, const sf::Vector3f& position );

		/** Get hook.
		 * @param id ID.
		 * @return Hook position or null if hook not found.
		 */
		const sf::Vector3f* get_hook( const std::string& id ) const;

	private:
		typedef std::map<const std::string, sf::Vector3f> HookMap;
		typedef std::vector<Resource> ResourceVector;

		FlexId m_id;
		std::string m_name;
		sf::Vector3f m_origin;

		HookMap m_hooks;
		ResourceVector m_textures;
};

}
