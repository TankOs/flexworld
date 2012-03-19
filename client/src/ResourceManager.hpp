#pragma once

#include "BufferObjectGroup.hpp"

#include <FlexWorld/FlexID.hpp>
#include <FlexWorld/Model.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <boost/thread.hpp>
#include <string>
#include <map>
#include <memory>

namespace sg {
class BufferObject;
}

/** Resource manager.
 * Manages textures and models. All load and find operations are thread-safe.
 */
class ResourceManager {
	public:
		/** Set base path.
		 * @param path Path.
		 */
		void set_base_path( const std::string& path );

		/** Prepare a texture.
		 * Loads the image data from disk but does not create the OpenGL texture
		 * yet. This is useful when loading resources from threads without an
		 * active OGL context.
		 * If a texture with the same ID is found, it's being removed.
		 * @param id ID.
		 * @return true on success, false on error.
		 * @see finalize_prepared_textures for finalizing prepared textures, i.e. loading image data into them.
		 */
		bool prepare_texture( const flex::FlexID& id );

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

		/** Load model from file.
		 * A model with the same ID gets removed (even if loading fails).
		 * @param id ID.
		 * @return true on success.
		 */
		bool load_model( const flex::FlexID& id );

		/** Find model.
		 * @param id ID.
		 * @return Model or nullptr if not found.
		 */
		std::shared_ptr<const flex::Model> find_model( const flex::FlexID& id ) const;

		/** Prepare buffer object group.
		 * The buffer objects are uninitialized until
		 * finalize_prepared_buffer_object_groups() is called. However they can
		 * already been used, just nothing will be rendered.
		 * The model will be loaded automagically if not done before.
		 * @param model_id ID of model.
		 * @return true on success, false if failed to load model.
		 */
		bool prepare_buffer_object_group( const flex::FlexID& model_id );

		/** Find buffer object group.
		 * @param model_id Model ID.
		 * @return Buffer object group or nullptr if not found.
		 */
		BufferObjectGroup::PtrConst find_buffer_object_group( const flex::FlexID& model_id ) const;

		/** Garbage collect.
		 * All resources with a use count of 1 are unloaded.
		 */
		void garbage_collect();

		/** Finalize prepared textures.
		 */
		void finalize_prepared_textures();

		/** Finalize prepared buffer objects.
		 */
		void finalize_prepared_buffer_objects();

	private:
		typedef std::shared_ptr<sf::Texture> TexturePtr;
		typedef std::shared_ptr<flex::Model> ModelPtr;
		typedef std::shared_ptr<const flex::Model> ModelPtrConst;
		typedef std::shared_ptr<sf::Image> ImagePtr;

		typedef std::map<const std::string, TexturePtr> TextureMap;
		typedef std::map<const std::string, ModelPtr> ModelMap;
		typedef std::map<const std::string, BufferObjectGroup::Ptr> BufferObjectGroupMap;
		typedef std::map<const std::string, ImagePtr> PreparedTextureMap;
		typedef std::map<const std::string, ModelPtrConst> PreparedBufferObjectGroupMap;

		PreparedTextureMap m_prepared_textures;
		PreparedBufferObjectGroupMap m_prepared_buffer_object_groups;

		TextureMap m_textures;
		ModelMap m_models;
		BufferObjectGroupMap m_buffer_object_groups;

		mutable boost::mutex m_textures_mutex;
		mutable boost::mutex m_models_mutex;
		mutable boost::mutex m_buffer_object_groups_mutex;

		std::string m_base_path;
};
