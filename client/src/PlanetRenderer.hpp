#pragma once

#include "BufferObject.hpp"
#include "Camera.hpp"

#include <FlexWorld/Planet.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <boost/thread.hpp>
#include <cstdint>

class ResourceManager;

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
		typedef uint32_t ChunkPosition;
		typedef std::map<ChunkPosition, std::size_t> ChunkVBOIndexMap;
		typedef std::map<std::shared_ptr<const sf::Texture>, ChunkVBOIndexMap> TextureChunkPositionMap;

		void pause();
		void resume();

		mutable boost::mutex m_render_mutex;

		VBOVector m_vbos;
		TextureChunkPositionMap m_chunk_positions;

		const Camera* m_camera;

		const flex::Planet& m_planet;
		ResourceManager& m_resource_manager;
};
