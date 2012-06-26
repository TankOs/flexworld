#pragma once

#include <FlexWorld/Cuboid.hpp>

#include <vector>
#include <cstdint>

namespace flex {

class Planet;
class Class;

/** Terrain generator.
 *
 * Terrain generator using Perlin noise.
 *
 * The generator fills the space up to base height with blocks completely. Set
 * base height to 0.0 to allow "holes".
 */
class TerrainGenerator {
	public:
		/** Layer.
		 */
		struct Layer {
			/** Ctor.
			 */
			Layer();

			uint32_t min_height; ///< Min height.
			uint32_t max_height; ///< Max height.
			const Class* cls; ///< Class.
		};

		/** Ctor.
		 * @param default_id Default class (reference is stored!).
		 */
		TerrainGenerator( const Class& default_cls );

		/** Get default class.
		 * @return Default class.
		 */
		const Class& get_default_class() const;

		/** Get number of layers.
		 * @return Number of layers.
		 */
		std::size_t get_num_layers() const;

		/** Add layer.
		 * @param layer Layer (is copied).
		 */
		void add_layer( const Layer& layer );

		/** Get layer.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Layer.
		 */
		const Layer& get_layer( std::size_t index ) const;

		/** Get seed.
		 * @return Seed.
		 */
		int get_seed() const;

		/** Set seed.
		 * @param seed Seed.
		 */
		void set_seed( int seed );

		/** Get base height.
		 * @return Base height.
		 */
		uint32_t get_base_height() const;

		/** Set base height.
		 * @param height Base height.
		 */
		void set_base_height( uint32_t height );

		/** Get maximum height.
		 * @return Maximum height.
		 */
		uint32_t get_maximum_height() const;

		/** Set maximum height.
		 * @param height Maximum height.
		 */
		void set_maximum_height( uint32_t height );

		/** Generate terrain.
		 * Previously set data on the planet is overwritten. Data is NOT cleared.
		 * @param planet Planet to store generated terrain at.
		 * @param cuboid Cuboid to generate (must be valid).
		 */
		void generate( Planet& planet, const Cuboid<uint32_t>& cuboid );

	private:
		typedef std::vector<Layer> LayerArray;

		const Class* m_default_cls;
		LayerArray m_layers;
		int m_seed;
		uint32_t m_base_height;
		uint32_t m_max_height;
};

bool operator==( const TerrainGenerator::Layer& first, const TerrainGenerator::Layer& second );

}
