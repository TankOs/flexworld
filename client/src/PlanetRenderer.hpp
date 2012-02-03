#pragma once

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
		PlanetRenderer( const flex::Planet& planet, const ResourceManager& resource_manager );

		/** Dtor.
		 */
		~PlanetRenderer();

		/** Render.
		 */
		void render() const;

		/** Prepare chunk.
		 * Completely prepare a chunk for rendering the next time render() is
		 * called.
		 * @param chunk_pos Chunk position.
		 */
		void prepare_chunk( const flex::Planet::Vector& chunk_pos );

	private:
		typedef std::vector<GLuint> VBOVector;
		typedef uint32_t ChunkPosition;
		typedef std::map<ChunkPosition, std::size_t> ChunkVBOIndexMap;
		typedef std::map<std::shared_ptr<const sf::Texture>, ChunkVBOIndexMap> TextureChunkPositionMap;

		const flex::Planet& m_planet;
		const ResourceManager& m_resource_manager;

		VBOVector m_vbos;
		TextureChunkPositionMap m_chunk_positions;
};
