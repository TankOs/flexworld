#pragma once

#include <SFML/System/Vector3.hpp>
#include <vector>
#include <map>
#include <cstdint>

#if 0

namespace flex {

class Class;

/** Chunk.
 * Contains blocks.
 */
class Chunk {
	public:
		typedef uint8_t SizeType; ///< Size type for chunk coordinates.
		typedef sf::Vector3<SizeType> Vector; ///< Vector.

		/** Ctor.
		 * @param size Size.
		 */
		Chunk( const Vector& size );

		/** Dtor.
		 */
		~Chunk();

		/** Clear chunk.
		 */
		void clear();

		/** Get size.
		 * @return Size.
		 */
		const Vector& get_size() const;

		/** Set block.
		 * @param pos Position.
		 * @param cls Class.
		 */
		void set_block( const Vector& pos, const Class& cls );

		/** Clear block.
		 * @param pos Position.
		 */
		void clear_block( const Vector& pos );

		/** Get a block's class.
		 * @param pos Position.
		 * @return Class or null.
		 */
		const Class* get_block_class( const Vector& pos ) const;

	private:
		typedef uint16_t ClassIdType;
		typedef std::vector<const Class*> ClassVector;

		typedef std::pair<ClassIdType, uint32_t> ClassIdCountPair;
		typedef std::map<const Class*, ClassIdCountPair> ClassIdCountMap; // TODO: unsorted_map/other?

		Vector m_size;
		ClassIdType* m_blocks;

		ClassVector m_classes;
		ClassIdCountMap m_ids;
};

}

#endif
