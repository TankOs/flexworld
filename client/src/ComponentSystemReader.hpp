#pragma once

#include <FlexWorld/Types.hpp>

#include <FWMS/Reader.hpp>
#include <map>

namespace fw {
class World;
class LockFacility;
}

namespace cs {
class System;
class Entity;
}

/** (FW) Component system reader.
 * Reader responsible for keeping the component system in sync and modifying it
 * depending on actions.
 */
class ComponentSystemReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		ComponentSystemReader();

		/** Set system.
		 * @param system System (referenced).
		 */
		void set_system( cs::System& system );

		/** Set world.
		 * @param world World (referenced).
		 */
		void set_world( const fw::World& world );

		/** Set lock facility.
		 * @param lock_facility Lock facility (referenced).
		 */
		void set_lock_facility( fw::LockFacility& lock_facility );

		/** Get controlled entity.
		 * @return Controlled entity or nullptr if no entity is being controlled.
		 */
		const cs::Entity* get_controlled_entity() const;

	private:
		void handle_message( const ms::Message& message );

		cs::System* m_system;
		const fw::World* m_world;
		fw::LockFacility* m_lock_facility;
		std::map<fw::EntityID, cs::Entity*> m_entities;

		fw::EntityID m_controlled_entity_id;
		bool m_controlling_entity;
};
