#pragma once

#include <FlexWorld/Types.hpp>

#include <FWMS/Reader.hpp>
#include <boost/thread.hpp>
#include <list>
#include <memory>

class PlanetDrawable;
class EntityGroupNode;

namespace fw {
class World;
class LockFacility;
}

namespace ms {
class Message;
}

/** Object preparer reader.
 * FWMS reader that listens for messages that concern chunk/entity creations
 * and updates (messages "chunk_invalidated" and "entity_invalidated").
 *
 * Listens for:
 *   * chunk_invalidated (fw::ChunkVector)
 *   * entity_invalidated (fw::ChunkVector)
 */
class ObjectPreparerReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		ObjectPreparerReader();

		/** Dtor.
		 */
		~ObjectPreparerReader();

		/** Check if worker thread is running.
		 * @return true if running.
		 */
		bool is_running() const;

		/** Launch worker thread.
		 * Undefined behaviour if already running.
		 * @see is_running
		 */
		void launch();

		/** Stop worker thread.
		 * This will block until the thread has been cleanly shutdown.
		 */
		void stop();

		/** Set planet drawable.
		 * The drawable will be used to finally prepare the objects.
		 * @param planet_drawable PlanetDrawable.
		 */
		void set_planet_drawable( std::shared_ptr<PlanetDrawable> planet_drawable );

		/** Set world.
		 * @param world World.
		 */
		void set_world( const fw::World& world );

		/** Set lock facility.
		 * @param lock_facility Lock facility.
		 */
		void set_lock_facility( fw::LockFacility& lock_facility );

		/** Set entity group node.
		 * @param entity_group_node Entity group node.
		 */
		void set_entity_group_node( std::shared_ptr<EntityGroupNode> entity_group_node );

	private:
		typedef std::list<fw::ChunkVector> ChunkList;
		typedef std::list<fw::EntityID> EntityList;

		void handle_message( const ms::Message& message );
		void thread_func();

		boost::thread m_thread;
		mutable boost::mutex m_mutex;
		mutable boost::mutex m_data_mutex;
		mutable boost::mutex m_condition_mutex;
		mutable boost::condition_variable m_condition;

		std::shared_ptr<PlanetDrawable> m_planet_drawable;
		std::shared_ptr<EntityGroupNode> m_entity_group_node;

		ChunkList m_chunks;
		EntityList m_entities;

		const fw::World* m_world;
		fw::LockFacility* m_lock_facility;

		bool m_is_running;
		bool m_terminate;
};
