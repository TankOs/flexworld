#pragma once

#include <FlexWorld/NonCopyable.hpp>

#include <SFML/System/Vector3.hpp>
#include <cstdint>

namespace flex {

class Class;

/** Chunk.
 * Contains blocks.
 */
class Chunk : public NonCopyable {
	public:
		typedef uint8_t ScalarType; ///< Size type for block coordinates.
		typedef uint16_t Block; ///< Block.
		typedef sf::Vector3<ScalarType> Vector; ///< Vector.

		static const Block MAX_BLOCK_ID; ///< Maximum allowed block class ID.

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

		/** Check if a block is set.
		 * @param pos Position.
		 * @return true if set.
		 */
		bool is_block_set( const Vector& pos ) const;

		/** Get size.
		 * @return Size.
		 */
		const Vector& get_size() const;

		/** Get block.
		 * Undefined behaviour if no block is set at given position.
		 * @param pos Position.
		 * @return Block (class ID).
		 * @see is_block_set
		 */
		Block get_block( const Vector& pos ) const;

		/** Set block.
		 * @param pos Position.
		 * @param id ID.
		 */
		void set_block( const Vector& pos, Block id );

		/** Reset block.
		 * @param pos Position.
		 */
		void reset_block( const Vector& pos );

		/** Get raw block data.
		 * @return Block data.
		 */
		const Block* get_raw_data() const;

	private:
		static const Block INVALID_BLOCK; ///< Invalid/unset block.

		Vector m_size;
		Block* m_blocks;
};

}
