#pragma once

#include <FlexWorld/Resource.hpp>

#include <SFML/System/Vector3.hpp>
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace flex {

/** FlexWorld class.
 *
 * A class describes properties for objects (blocks and entities) in FlexWorld.
 * Objects can be created by Lua scripts either by setting blocks or spawning
 * entities.
 *
 * Hooks can be used to be able to attach entities to another entity at a
 * specified position. Hooks beginning with __ (two underscores) are reserved
 * names and can only be used internally. The following reserved names exist:
 *
 *   * __container: Stowed away entities, e.g. for inventory entities.
 *
 * Entities that are attached to reserved hooks are not sent to the client.
 * They're mostly being used for special behaviour, like showing a container's
 * contents (__container hook).
 */
class Class {
	public:
		/** Ctor.
		 * @param id ID.
		 */
		explicit Class( const FlexID& id );

		/** Copy ctor.
		 * @param other Other class.
		 */
		Class( const Class& other );

		/** Assignment.
		 * @param other Other class.
		 * @return *this.
		 */
		const Class& operator=( const Class& other );

		/** Get ID.
		 * @return ID.
		 */
		const FlexID& get_id() const;

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

		/** Check if texture exists.
		 * @param index Index.
		 * @return true if it exists.
		 */
		bool has_texture( std::size_t index ) const;

		/** Get texture.
		 * @param index Index (must be valid).
		 * @return Resource.
		 * @see has_texture for checking if an index is valid.
		 */
		const Resource& get_texture( std::size_t index ) const;

		/** Get number of textures.
		 * @return Number of textures.
		 */
		std::size_t get_num_textures() const;

		/** Get number of hooks.
		 * @return Number of hooks.
		 */
		std::size_t get_num_hooks() const;

		/** Set hook.
		 * A hook with the same name gets replaced.
		 * @param id ID.
		 * @param position Position.
		 */
		void set_hook( const std::string& id, const sf::Vector3f& position );

		/** Find hook.
		 * @param id ID.
		 * @return Hook position or null if hook not found.
		 */
		const sf::Vector3f* find_hook( const std::string& id ) const;

		/** Check if class has a model assigned.
		 * @return true if model assigned.
		 */
		bool has_model() const;

		/** Get model.
		 * Undefined behaviour in case no model is set.
		 * @return Model.
		 */
		const Resource& get_model() const;

		/** Set model.
		 * @param model Model.
		 */
		void set_model( const Resource& model );

		/** Set scale.
		 * @param scale Scale.
		 */
		void set_scale( const sf::Vector3f& scale );

		/** Get scale.
		 * @return Scale.
		 */
		const sf::Vector3f& get_scale() const;

	private:
		typedef std::map<const std::string, sf::Vector3f> HookMap;
		typedef std::vector<Resource> ResourceVector;

		HookMap m_hooks;
		ResourceVector m_textures;
		FlexID m_id;
		std::unique_ptr<Resource> m_model;
		std::string m_name;
		sf::Vector3f m_origin;
		sf::Vector3f m_scale;
};

}
