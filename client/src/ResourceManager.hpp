#pragma once

#include <FlexWorld/FlexID.hpp>
#include <FlexWorld/Model.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <boost/thread.hpp>
#include <string>
#include <map>
#include <memory>

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

		/** Garbage collect.
		 * All resources with a use count of 1 are unloaded.
		 */
		void garbage_collect();

		/** Finalize prepared textures.
		 */
		void finalize_prepared_textures();

	private:
		typedef std::shared_ptr<sf::Texture> TexturePtr;
		typedef std::shared_ptr<flex::Model> ModelPtr;
		typedef std::shared_ptr<sf::Image> ImagePtr;

		typedef std::map<const std::string, TexturePtr> TextureMap;
		typedef std::map<const std::string, ModelPtr> ModelMap;
		typedef std::map<const std::string, ImagePtr> PreparedTextureMap;

		PreparedTextureMap m_prepared_textures;
		TextureMap m_textures;
		ModelMap m_models;

		mutable boost::mutex m_textures_mutex;
		mutable boost::mutex m_models_mutex;

		std::string m_base_path;
};
