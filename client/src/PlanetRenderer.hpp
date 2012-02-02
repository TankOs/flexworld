#pragma once

#include <FlexWorld/Planet.hpp>

#include <boost/thread.hpp>
#include <list>

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

		/** Render.
		 */
		void render() const;

		/** Prepare chunk.
		 * Preparing a chunk will prepare vertices and texture coordinates without
		 * issueing a single OpenGL command. It's therefore save to call it from a
		 * thread that's hasn't got an active OpenGL context.
		 * @param chunk_pos Chunk position.
		 */
		void prepare_chunk( const flex::Planet::Vector& chunk_pos );

	private:
		const flex::Planet& m_planet;
		const ResourceManager& m_resource_manager;
};
