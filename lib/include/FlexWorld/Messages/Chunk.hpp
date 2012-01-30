#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Chunk.hpp>
#include <FlexWorld/Planet.hpp>

#include <vector>
#include <string>

namespace flex {
namespace msg {

/** Chunk network message.
 * The message includes two groups of data:
 *
 * -# Used class IDs (may be plain IDs or SHA-1 hashes, max. 255 chars per
 *  class ID, max. 0x0fff class IDs).
 * -# Block data (one WORD each), where the first 4 most-significant bits
 *  represent flags and the remaining 12 bits define the class ID index of the
 *  class IDs specified in group 1. Max. blocks: 0xffff.
 */
class Chunk : public Message {
	public:
		/** Ctor.
		 */
		Chunk();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get number of blocks.
		 * @return Number of blocks.
		 */
		std::size_t get_num_blocks() const;

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const Planet::Vector& pos );

		/** Get position.
		 * @return Position.
		 */
		const Planet::Vector& get_position() const;

		/** Set blocks.
		 * @param chunk Chunk to extract blocks from.
		 */
		void set_blocks( const flex::Chunk& chunk );

		/** Get block.
		 * @param index Block index (must be valid).
		 * @return Block.
		 */
		flex::Chunk::Block get_block( std::size_t index ) const;

	private:
		typedef std::vector<flex::Chunk::Block> BlockVector;
		typedef uint16_t NumBlocksType;

		BlockVector m_blocks;
		Planet::Vector m_position;
};

}
}
