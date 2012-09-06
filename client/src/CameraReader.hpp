#pragma once

#include <FlexWorld/Types.hpp>

#include <FWMS/Reader.hpp>

namespace sg {
class Camera;
}

namespace fw {
class World;
class LockFacility;
}

/** Camera reader.
 * Attaches the camera to an entity and updates its position accordingly.
 *
 * The reader waits for a message "control_entity" and attaches the camera to
 * the entity ID given in the message.
 */
class CameraReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		CameraReader();

		/** Set camera.
		 * @param camera Camera (referenced).
		 */
		void set_camera( sg::Camera& camera );

		/** Set world.
		 * @param world World (referenced).
		 */
		void set_world( const fw::World& world );

		/** Set lock facility.
		 * @param lock_facility Lock facility (referenced).
		 */
		void set_lock_facility( fw::LockFacility& lock_facility );

	private:
		void handle_message( const ms::Message& message );

		sg::Camera* m_camera;
		const fw::World* m_world;
		fw::LockFacility* m_lock_facility;

		fw::EntityID m_entity_id;
		bool m_received_entity_id;
};
