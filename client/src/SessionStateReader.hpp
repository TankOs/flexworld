#pragma once

#include <FWMS/Reader.hpp>

class SessionState;

namespace fw {
class World;
class LockFacility;
}

/** Session state reader.
 * Updates the client's session state upon certain actions.
 */
class SessionStateReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		SessionStateReader();

		/** Set session state.
		 * @param session_state Session state.
		 */
		void set_session_state( SessionState& session_state );

		/** Set world.
		 * @param world World.
		 */
		void set_world( const fw::World& world );

		/** Set lock facility.
		 * @param lock_facility Lock facility.
		 */
		void set_lock_facility( fw::LockFacility& lock_facility );

	private:
		void handle_message( const ms::Message& message );

		SessionState* m_session_state;
		const fw::World* m_world;
		fw::LockFacility* m_lock_facility;
};
