#pragma once

#include <FlexWorld/Mesh.hpp>

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

	private:
		typedef std::vector<Mesh> MeshVector;

		MeshVector m_meshes;
		float m_scale_divisor;
};

}
