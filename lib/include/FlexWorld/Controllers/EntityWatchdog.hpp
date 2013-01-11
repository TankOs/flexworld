#pragma once

#include <FlexWorld/Types.hpp>

#include <FWCS/Controller.hpp>
#include <FWCS/ControllerRequirements.hpp>
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
 *   * snapshot (const EntityWatchdog::Snapshot*): Snapshot containing the changes (CONST POINTER!).
 *
 * Required properties:
 *   * fw_entity_id (fw::EntityID, in): FlexWorld entity ID.
 *   * watch_router (ms::Router*, in): FWMS router to pass messages to.
 *   * position (sf::Vector3f, in): Position.
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
		};

		/** Get requirements.
		 * @return Requirements.
		 */
		static const cs::ControllerRequirements& get_requirements();

		/** Ctor.
		 * @param entity Entity.
		 */
		EntityWatchdog( cs::Entity& entity );

		/** Execute.
		 * @param sim_time Simulation time.
		 */
		void execute( const sf::Time& sim_time );

	private:
		Snapshot m_snapshot;

		const fw::EntityID* m_fw_entity_id;
		ms::Router** m_watch_router;
		const sf::Vector3f* m_position;
};

}
}
