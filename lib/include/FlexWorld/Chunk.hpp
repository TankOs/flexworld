#pragma once

#include <FlexWorld/ClassCache.hpp>
#include <FlexWorld/NonCopyable.hpp>

#include <SFML/System/Vector3.hpp>
#include <vector>
#include <map>
#include <cstdint>

namespace flex {

class Class;

/** Chunk.
 * Contains blocks.
 */
class Chunk : public NonCopyable {
	public:
		typedef uint8_t ScalarType; ///< Size type for block coordinates.
		typedef sf::Vector3<ScalarType> Vector; ///< Vector.

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

		/** Get block.
		 * @param pos Position.
		 * @return Internal class ID (0 = error).
		 */
		ClassCache::IdType get_block( const Vector& pos ) const;

		/** Set block.
		 * @param pos Position.
		 * @param class_id Internal class ID.
		 */
		void set_block( const Vector& pos, ClassCache::IdType class_id );

	private:
		Vector m_size;
		ClassCache::IdType* m_blocks;
};

}
