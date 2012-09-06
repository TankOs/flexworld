#pragma once

#include <FlexWorld/Types.hpp>

#include <FWCS/Controller.hpp>
#include <SFML/System/Vector3.hpp>
#include <map>

namespace ms {
class Router;
}

namespace fw {
namespace ctrl {

/** Entity watchdog.
 * The watchdog watches for changes in entities. Changes include:
 *
 *   * Position
 *
 * Whenever a change is detected, the watchdog will create a message and pass
 * it to the message system. The message's ID is "entity_change" and contains
 * the following properties:
 *
 *   * id (fw::EntityID): FlexWorld entity ID.
 *   * fields (int): Flags of properties that changed (see fw::ctrl::EntityWatchdog::ChangeFieldFlag).
 *   * snapshot (EntityWatchdog::Snapshot): Snapshot containing the changes.
 *
 * Required properties:
 *   * watch (bool): true to enable watching this entity.
 */
class EntityWatchdog : public cs::Controller {
	public:
		/** Flags that specify which fields got changed.
		 */
		enum ChangeFieldFlag {
			UNCHANGED = 0, ///< Nothing changed.
			POSITION = 1 << 0, ///< Position changed.
			ALL = POSITION ///< All fields.
		};

		/** Entity snapshot.
		 */
		struct Snapshot {
			sf::Vector3f position; ///< Position.

			/** Equality.
			 * @param other Other snapshot.
			 * @return true if equal.
			 */
			bool operator==( const Snapshot& other ) const;
		};

		/** Ctor.
		 */
		EntityWatchdog();

		/** Get number of entity snapshots.
		 * @return Number of snapshots.
		 */
		std::size_t get_num_snapshots() const;

		/** Find snapshot.
		 * @param entity_id Entity ID.
		 * @return Snapshot or nullptr if not found.
		 */
		const Snapshot* find_snapshot( fw::EntityID entity_id ) const;

		/** Set router.
		 * @param router Router (referenced).
		 */
		void set_router( ms::Router& router );

		/** Get router.
		 * @return Router or nullptr if none set.
		 */
		ms::Router* get_router() const;

	private:
		void on_entity_add( cs::Entity& entity );
		void on_entity_remove( cs::Entity& entity );
		void update_entity( cs::Entity& entity, const sf::Time& delta );

		std::map<fw::EntityID, Snapshot> m_snapshots;

		ms::Router* m_router;
};

}
}
