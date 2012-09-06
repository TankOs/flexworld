#pragma once

#include <FlexWorld/Types.hpp>

#include <FWCS/Controller.hpp>
#include <SFML/System/Vector3.hpp>
#include <functional>
#include <map>

namespace fw {
namespace ctrl {

/** Entity watchdog.
 * The watchdog watches for changes in entities. Changes include:
 *
 *   * Position
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
			POSITION = 1 << 0 ///< Position changed.
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

		/** Set callback function.
		 * @param function Function.
		 */
		void set_callback_function( const std::function<void( fw::EntityID, int )>& function );

		/** Get callback function.
		 * @return Callback function.
		 */
		const std::function<void( fw::EntityID, int )>& get_callback_function() const;

		/** Find snapshot.
		 * @param entity_id Entity ID.
		 * @return Snapshot or nullptr if not found.
		 */
		const Snapshot* find_snapshot( fw::EntityID entity_id ) const;

	private:
		void on_entity_add( cs::Entity& entity );
		void on_entity_remove( cs::Entity& entity );
		void update_entity( cs::Entity& entity, const sf::Time& delta );

		std::function<void( fw::EntityID, int )> m_callback_function;
		std::map<fw::EntityID, Snapshot> m_snapshots;
};

}
}
