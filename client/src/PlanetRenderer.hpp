#pragma once

#include "BufferObject.hpp"
#include "Camera.hpp"

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Face.hpp>
#include <FlexWorld/Chunk.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/Model.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <boost/thread.hpp>
#include <map>
#include <vector>
#include <cstdint>

class ResourceManager;

namespace flex {
class Model;
class FlexID;
}

/** Planet renderer.
 *
 * This renderer renders a planet's chunks.
 */
class PlanetRenderer {
	public:
		/** Ctor.
		 * @param planet Planet.
		 * @param resource_manager Resource manager.
		 */
		PlanetRenderer( const flex::Planet& planet, ResourceManager& resource_manager );

		/** Dtor.
		 */
		~PlanetRenderer();

		/** Render.
		 */
		void render() const;

		/** Prepare chunk.
		 * Completely prepare a chunk for rendering the next time render() is
		 * called. This will also issue the resource manager in case a model or
		 * texture is not loaded.
		 * @param chunk_pos Chunk position.
		 */
		void prepare_chunk( const flex::Planet::Vector& chunk_pos );

		/** Set camera.
		 * @param camera Camera.
		 */
		void set_camera( const Camera& camera );

	private:
		typedef std::vector<std::shared_ptr<BufferObject>> VBOVector;
		typedef std::map<std::shared_ptr<const sf::Texture>, VBOVector> TextureVBOVectorMap;

		typedef uint32_t ChunkPosition;
		typedef std::map<std::shared_ptr<const sf::Texture>, std::size_t> TextureVBOIndexMap;
		typedef std::map<ChunkPosition, TextureVBOIndexMap> ChunkTextureMap;

		void pause();
		void resume();

		std::shared_ptr<const flex::Model> get_model( const flex::FlexID& id ) const;
		std::shared_ptr<const sf::Texture> get_texture( const flex::FlexID& id ) const;

		mutable boost::mutex m_render_mutex;

		TextureVBOVectorMap m_vbos;
		ChunkTextureMap m_chunk_textures;

		const Camera* m_camera;

		const flex::Planet& m_planet;
		ResourceManager& m_resource_manager;
};
