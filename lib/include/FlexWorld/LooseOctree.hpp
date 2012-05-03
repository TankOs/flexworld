#pragma once

#include <FlexWorld/Exception.hpp>

#include <SFML/System/Vector3.hpp>
#include <map>
#include <list>
#include <cstdint>

namespace flex {

/** Loose octree.
 *
 * A loose octree is like a normal octree with the difference that each node's
 * bounds are loose and scaled by a factor of 2. Therefore it's guaranteed that
 * volumetric data with dimensions <= root node dimensions fits into a single
 * node.
 *
 * The tree uses copy semantics, that means inserted data objects will be
 * copied into the tree. If you're storing complex data using pointers to the
 * data may be easier on the memory footprint.
 *
 * DT: Data type.
 */
template <class DT>
class LooseOctree {
	public:
		/** Quadrant.
		 */
		enum Quadrant {
			LEFT_TOP_FAR = 0,
			RIGHT_TOP_FAR,
			LEFT_TOP_NEAR,
			RIGHT_TOP_NEAR,
			LEFT_BOTTOM_FAR,
			RIGHT_BOTTOM_FAR,
			LEFT_BOTTOM_NEAR,
			RIGHT_BOTTOM_NEAR,
			SAME_QUADRANT,
			INVALID_QUADRANT
		};

		typedef uint32_t Size; ///< Size type.
		typedef sf::Vector3<Size> Vector; ///< Tree location vector.

		/** Data info.
		 */
		struct DataInfo {
			/** Ctor.
			 */
			DataInfo();

			DT data; ///< Data.
			Vector position; ///< Position (center point).
			Size size; ///< Size.
		};

		typedef std::list<DataInfo> DataList; ///< List of data.

		/** Ctor.
		 * Position is initialized to 0, 0, 0.
		 * @param size Size (must be power of two).
		 */
		LooseOctree( Size size );

		/** Dtor.
		 */
		~LooseOctree();

		/** Get size.
		 * @return Size.
		 */
		Size get_size() const;

		/** Get position.
		 * @return Position.
		 */
		const Vector& get_position() const;

		/** Check if node is subdivided.
		 * @return true if subdivided.
		 */
		bool is_subdivided() const;

		/** Get number of data *for this node*.
		 * @return Number of data.
		 */
		std::size_t get_num_data() const;

		/** Check if node has a child.
		 * @param quadrant Quadrant.
		 * @return true if child exists, false if not or not subdivided.
		 */
		bool has_child( Quadrant quadrant ) const;

		/** Get child.
		 * Undefined behaviour if child doesn't exist.
		 * @param quadrant Quadrant.
		 * @see has_child
		 */
		LooseOctree<DT>& get_child( Quadrant quadrant ) const;

		/** Get data.
		 * Undefined behaviour if there's no data.
		 * @return Data.
		 * @see get_num_data
		 */
		const DataList& get_data() const;

		/** Insert data.
		 * Undefined behaviour if either center point is out of bounds or size is
		 * bigger than node's size.
		 * @param data Data.
		 * @param center Center point.
		 * @param size Size.
		 * @return Node the data has been added to.
		 */
		LooseOctree& insert( const DT& data, const Vector& center, Size size );

	private:
		LooseOctree( const Vector& position, Size size );

		Quadrant determine_quadrant( const Vector& center, Size size );
		void ensure_data();
		void subdivide();
		void create_child( Quadrant quadrant );

		Vector m_position;

		DataList* m_data;
		LooseOctree** m_children;

		Size m_size;
};

}

#include "LooseOctree.inl"
