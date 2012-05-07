#pragma once

#include <FlexWorld/NonCopyable.hpp>
#include <FlexWorld/Chunk.hpp>
#include <FlexWorld/ClassCache.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/LooseOctree.hpp>

#include <SFML/System/Vector3.hpp>
#include <string>
#include <map>
#include <vector>

namespace flex {

/** Planet.
 * 
 * A planet in FlexWorld is a fixed-size map. It contains chunks of block data
 * and entities.
 *
 * Block data is stored as cached class IDs to lower the memory footprint.
 *
 * The planet also keeps track of entities and manages a loose octree to
 * provide fast searches (e.g. for collision detection). When an entity's
 * transformation is changed make sure to call update_entity() so that proper
 * node in the octree is updated.
 */
class Planet : public NonCopyable {
	public:
		typedef uint16_t ScalarType; ///< Scalar type for planet size/chunk positions.
		typedef sf::Vector3<ScalarType> Vector; ///< Vector for planet size/chunk positions.
		typedef sf::Vector3<float> Coordinate; ///< Vector for positions in absolute planet coordinates.
		typedef std::vector<Entity::ID> EntityIDArray; ///< Array of entity IDs.

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
		 * Removes all chunks and entities. The planet's properties are kept.
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

		/** Add entity.
		 * Only the entity's ID is stored. Undefined behaviour if ID has already
		 * been added. Use has_entity() to check.
		 * @param entity Entity.
		 */
		void add_entity( const Entity& entity );

		/** Check if entity was added.
		 * @param entity Entity.
		 * @return true if added, false otherwise.
		 */
		bool has_entity( const Entity& entity ) const;

		/** Remove entity.
		 * Undefined behaviour if entity hasn't been added before. Check with
		 * has_entity().
		 * @param entity Entity.
		 */
		void remove_entity( const Entity& entity );

		/** Get nth entity ID.
		 * Undefined behaviour if index invalid.
		 * @param index Index.
		 * @return Entity ID.
		 */
		Entity::ID get_entity_id( std::size_t index ) const;

		/** Get number of entities.
		 * @return Number of entities.
		 */
		std::size_t get_num_entities() const;

		/** Get raw chunk data.
		 * @param position Position (must be valid).
		 * @return Raw chunk data.
		 */
		const Chunk::Block* get_raw_chunk_data( const Planet::Vector& position ) const;

		/** Search for entities in a cuboid.
		 * @param cuboid Cuboid.
		 * @param results Array being filled with found entity IDs (not cleared!).
		 */
		void search_entities( const FloatCuboid& cuboid, EntityIDArray& results ) const;

	private:
		typedef std::map<const Vector, Chunk*> ChunkMap;
		typedef LooseOctree<Entity::ID, float> EntityOctree;
		typedef std::map<Entity::ID, EntityOctree*> EntityNodeMap;

		Vector m_size;
		Chunk::Vector m_chunk_size;
		std::string m_id;

		ChunkMap m_chunks;
		EntityIDArray m_entities;
		ClassCache m_class_cache;

		EntityNodeMap m_entity_nodes;
		LooseOctree<Entity::ID, float> m_octree;
};

}
