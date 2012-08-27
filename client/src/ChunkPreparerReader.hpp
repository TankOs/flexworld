#pragma once

#include <FlexWorld/Types.hpp>

#include <FWMS/Reader.hpp>
#include <boost/thread.hpp>
#include <list>
#include <memory>

class PlanetDrawable;

namespace ms {
class Message;
}

/** Chunk preparer reader.
 * FWMS reader that listens for messages that concern chunk creations and
 * updates (message "chunk_created" and "chunk_updated").
 *
 * The reader will (re)create the chunk in memory (its geometry) and enqueue a
 * message "chunk_prepared" when completed.
 *
 * Listens for:
 *   * chunk_created (fw::ChunkVector)
 *   * chunk_updated (fw::ChunkVector)
 *
 * Creates:
 *   * chunk_prepared (fw::ChunkVector)
 */
class ChunkPreparerReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		ChunkPreparerReader();

		/** Dtor.
		 */
		~ChunkPreparerReader();

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
		 * The drawable will be used to finally prepare the chunks.
		 * @param planet_drawable PlanetDrawable.
		 */
		void set_planet_drawable( std::shared_ptr<PlanetDrawable> planet_drawable );

		/** Get planet drawable.
		 * @return PlanetDrawable or nullptr if not set.
		 */
		std::shared_ptr<PlanetDrawable> get_planet_drawable() const;

	private:
		typedef std::list<fw::ChunkVector> ChunkList;

		void handle_message( const ms::Message& message );
		void thread_func();

		boost::thread m_thread;
		mutable boost::mutex m_mutex;
		mutable boost::mutex m_data_mutex;
		mutable boost::mutex m_condition_mutex;
		mutable boost::condition_variable m_condition;

		std::shared_ptr<PlanetDrawable> m_planet_drawable;

		ChunkList m_chunks;

		bool m_is_running;
		bool m_terminate;
};
