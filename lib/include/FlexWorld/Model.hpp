#pragma once

#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Cuboid.hpp>
#include <FlexWorld/Face.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <vector>

namespace flex {

/** 3D model.
 * The model consists of meshes.
 */
class Model {
	public:
		/** Ctor.
		 */
		Model();

		/** Get number of meshes.
		 * @return Number of meshes.
		 */
		std::size_t get_num_meshes() const;

		/** Add mesh (copy).
		 * @param mesh Mesh.
		 */
		void add_mesh( const Mesh& mesh );

		/** Get mesh.
		 * Undefined behaviour if index out of range.
		 * @param index Index.
		 * @return Mesh.
		 */
		const Mesh& get_mesh( std::size_t index ) const;

		/** Set block scale divisor.
		 * @param divisor Divisor (clamped to 1.0..INF).
		 */
		void set_block_scale_divisor( float divisor );

		/** Get block scale divisor.
		 * @return Scale divisor.
		 */
		float get_block_scale_divisor() const;

		/** Set bounding box.
		 * Position must be positive or zero.
		 * @param box Bounding box.
		 */
		void set_bounding_box( const FloatCuboid& box );

		/** Get bounding box.
		 * @return Bounding box.
		 */
		const FloatCuboid& get_bounding_box() const;

		/** Set face coverage.
		 * Mappings:
		 *
		 * * x, z -> x, y
		 * * x, y -> x, y
		 * * y, z -> x, y
		 *
		 * @param face Face (must not be NUM_FACES).
		 * @param coverage Coverage (left/top must be zero or positive; width/height must be zero or positive see description for axis mappings).
		 */
		void set_face_coverage( Face face, const sf::FloatRect& coverage );

		/** Get face coverage.
		 * @param face Face (must not be NUM_FACES).
		 * @return Face coverage.
		 */
		const sf::FloatRect& get_face_coverage( Face face ) const;

	private:
		typedef std::vector<Mesh> MeshVector;

		FloatCuboid m_bounding_box;
		MeshVector m_meshes;
		float m_scale_divisor;

		sf::FloatRect m_coverages[NUM_FACES];
};

}
