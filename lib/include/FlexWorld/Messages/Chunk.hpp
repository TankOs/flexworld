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

		/** Add block.
		 * If the given class ID is already used, it's being referenced internally.
		 * @param class_id Class ID (must be non-empty).
		 * @param flags Flags (only the 4 least-significant bits are used).
		 */
		void add_block( const std::string& class_id, uint8_t flags );

		/** Get a block's class ID.
		 * @param index Block index (must be valid).
		 * @return Class ID.
		 */
		const std::string& get_block_class_id( std::size_t index ) const;

		/** Get a block's flags.
		 * @param index Block index (must be valid).
		 * @return Flags.
		 */
		uint8_t get_block_flags( std::size_t index ) const;

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const Planet::Vector& pos );

		/** Get position.
		 * @return Position.
		 */
		const Planet::Vector& get_position() const;

	private:
		typedef std::vector<std::string> ClassIDVector;
		typedef std::vector<uint16_t> BlockVector;

		ClassIDVector m_class_ids;
		BlockVector m_blocks;
		Planet::Vector m_position;
};

}
}
