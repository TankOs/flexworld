#pragma once

#include <FWSG/TriangleGeometry.hpp>
#include <vector>
#include <cstdint>

namespace flex {

/** 3D mesh.
 */
class Mesh {
	public:
		typedef uint8_t TextureSlot; ///< Texture slot.

		/** Ctor.
		 */
		Mesh();

		/** Get geometry.
		 * @return Geometry.
		 */
		sg::TriangleGeometry& get_geometry();

		/** Get geometry.
		 * @return Geometry.
		 */
		const sg::TriangleGeometry& get_geometry() const;

		/** Get texture slot.
		 * @return Texture slot.
		 */
		TextureSlot get_texture_slot() const;

		/** Set texture slot.
		 * @param slot Slot.
		 */
		void set_texture_slot( TextureSlot slot );

		/** Clear.
		 */
		void clear();

	private:
		sg::TriangleGeometry m_geometry;
		TextureSlot m_texture_slot;
};

}
