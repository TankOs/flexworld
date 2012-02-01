#pragma once

#include <FlexWorld/Planet.hpp>

#include <boost/thread.hpp>
#include <list>

/** Planet renderer.
 *
 * This renderer renders a planet's chunks.
 */
class PlanetRenderer {
	public:
		/** Ctor.
		 */
		PlanetRenderer();

		/** Set planet.
		 * This erases ALL previously prepared data. The planet itself is only
		 * queued in the update() function, so make sure to lock the planet when
		 * calling it.
		 * @param planet Planet (reference stored).
		 */
		void set_planet( const flex::Planet& planet );

		/** Prepare chunk.
		 * Preparing a chunk will prepare vertices and texture coordinates without
		 * issueing a single OpenGL command. It's therefore save to call it from a
		 * thread that's hasn't got an active OpenGL context.
		 * @param chunk_pos Chunk position.
		 */
		void prepare_chunk( const flex::Planet::Vector& chunk_pos );

	private:
		const flex::Planet* m_planet;
};
