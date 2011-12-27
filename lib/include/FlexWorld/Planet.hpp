#pragma once

#include <FlexWorld/NonCopyable.hpp>
#include <FlexWorld/Chunk.hpp>
#include <FlexWorld/ClassCache.hpp>

#include <SFML/System/Vector3.hpp>
#include <map>
#include <string>

namespace flex {

/** Planet.
 * Contains chunks and class cache.
 */
class Planet : public NonCopyable {
	public:
		typedef uint16_t ScalarType; ///< Scalar type for planet size/chunk positions.
		typedef sf::Vector3<ScalarType> Vector; ///< Vector for planet size/chunk positions.
		typedef sf::Vector3<float> Coordinate; ///< Vector for positions in absolute planet coordinates.

		/** Ctor.
		 * @param id ID.
		 * @param size Size in chunks.
		 * @param chunk_size Chunk size in blocks.
		 */
		Planet( const std::string& id, const Vector& size, const Chunk::Vector& chunk_size );

		/** Dtor.
		 */
		~Planet();

		/** Clear.
		 */
		void clear();

		/** Get ID.
		 * @return ID.
		 */
		const std::string& get_id() const;

		/** Get size.
		 * @return Size.
		 */
		const Vector& get_size() const;

		/** Get chunk size.
		 * @return Chunk size.
		 */
		const Chunk::Vector& get_chunk_size() const;

		/** Transform planet coordinate into chunk and block positions.
		 * The transformation will always succeed in terms of setting the chunk and
		 * block position. However make sure to check for the return value to see
		 * if the resulting coordinates are within the planet's space (i.e. valid).
		 * @param coord Planet coordinate.
		 * @param chunk_pos Resulting chunk position.
		 * @param block_pos Resulting block position.
		 * @return false if planet coordinate is invalid.
		 */
		bool transform( const Coordinate& coord, Vector& chunk_pos, Chunk::Vector& block_pos ) const;

		/** Check if chunk exists.
		 * @param position Chunk position.
		 * @return true if it exists, false otherwise.
		 */
		bool has_chunk( const Vector& position ) const;

		/** Create chunk.
		 * This method fails if a chunk at the given position exists.
		 * @param position Chunk position (must be valid).
		 */
		void create_chunk( const Vector& position );

		/** Get number of chunks currently in memory.
		 * @return Number of chunks.
		 */
		std::size_t get_num_chunks() const;

		/** Set block.
		 * @param chunk_pos Chunk position (must be valid).
		 * @param block_pos Block position (must be valid).
		 * @param cls Class.
		 */
		void set_block( const Vector& chunk_pos, const Chunk::Vector& block_pos, const Class& cls );

		/** Find block.
		 * @param chunk_pos Chunk position (must be valid).
		 * @param block_pos Block position (must be valid).
		 * @return Class if block is set, nullptr otherwise.
		 */
		const Class* find_block( const Vector& chunk_pos, const Chunk::Vector& block_pos ) const;

		/** Reset (unset) block.
		 * The method succeeds whether a block is set or not. To differentiate, see
		 * find_block().
		 * @param chunk_pos Chunk position (must be valid).
		 * @param block_pos Block position (must be valid).
		 */
		void reset_block( const Vector& chunk_pos, const Chunk::Vector& block_pos );

	private:
		typedef std::map<const Vector, Chunk*> ChunkMap;

		Vector m_size;
		Chunk::Vector m_chunk_size;
		std::string m_id;

		ChunkMap m_chunks;
		ClassCache m_class_cache;
};

}
