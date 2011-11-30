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

		/** Get chunk.
		 * @param pos Position.
		 * @return Chunk or nullptr.
		 */
		const Chunk* get_chunk( const Vector& pos ) const;

		/** Get number of chunks currently in memory.
		 * @return Number of chunks.
		 */
		std::size_t get_num_chunks() const;

		/** Transform planet coordinate into chunk and block positions.
		 * @param coord Planet coordinate.
		 * @param chunk_pos Chunk position.
		 * @param block_pos Block position.
		 * @return false if planet coordinate is invalid.
		 */
		bool transform_coordinate( const Coordinate& coord, Vector& chunk_pos, Chunk::Vector& block_pos ) const;

		/** Get block.
		 * @param coord Planet coordinate.
		 * @return Class or nullptr.
		 */
		const Class* get_block( const Coordinate& coord ) const;

		/** Get class cache.
		 * @return Class cache.
		 */
		const ClassCache& get_class_cache() const;

		/** Set block.
		 * @param coord Planet coordinate.
		 * @param cls Class.
		 * @return true on success.
		 */
		bool set_block( const Coordinate& coord, const Class& cls );

		/** Create chunk.
		 * @param pos Chunk position.
		 * @return false when chunk already existant.
		 */
		bool create_chunk( const Vector& pos );

		/** Reset (delete) block.
		 * @param coord Planet coordinates.
		 * @return false when block not set/invalid chunk.
		 */
		bool reset_block( const Coordinate& coord );

	private:
		typedef std::map<const Vector, Chunk*> ChunkMap;

		Vector m_size;
		Chunk::Vector m_chunk_size;
		std::string m_id;

		ChunkMap m_chunks;

		ClassCache m_class_cache;
};

}
