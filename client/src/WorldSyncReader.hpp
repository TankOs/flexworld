#pragma once

#include <FWMS/Reader.hpp>

namespace fw {
class World;
class LockFacility;
}

/** Host synchronization reader.
 * Reader responsible for keeping the local world in sync with changes, for
 * example when entities move.
 */
class WorldSyncReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		WorldSyncReader();

		/** Set world.
		 * @param world World (referenced).
		 */
		void set_world( fw::World& world );

		/** Set lock facility.
		 * @param lock_facility Lock facility (referenced).
		 */
		void set_lock_facility( fw::LockFacility& lock_facility );

	private:
		void handle_message( const ms::Message& message );

		fw::World* m_world;
		fw::LockFacility* m_lock_facility;
};
