#pragma once

#include <FlexWorld/RefLock.hpp>

#include <map>

namespace flex {

class Planet;

/** Lock facility for securing several backend objects.
 *
 * Uses RefLock objects internally, i.e. it's safe to do multiple locks from
 * the same thread. Do not forget to unlock! ;-)
 */
class LockFacility {
	public:
		/** Lock or unlock account manager.
		 * @param do_lock true to lock, false to unlock.
		 */
		void lock_account_manager( bool do_lock );

		/** Check if account manager is locked.
		 * @return true when locked.
		 */
		bool is_account_manager_locked() const;

		/** Lock or unlock world.
		 * @param do_lock true to lock, false to unlock.
		 */
		void lock_world( bool do_lock );

		/** Check if world is locked.
		 * @return true when locked.
		 */
		bool is_world_locked() const;

		/** Lock or unlock planet.
		 * @param planet Planet.
		 * @param do_lock true to lock, false to unlock.
		 */
		void lock_planet( const Planet& planet, bool do_lock );

		/** Check if planet is locked.
		 * @param planet Planet.
		 * @return true when locked.
		 */
		bool is_planet_locked( const Planet& planet ) const;

	private:
		typedef std::map<const Planet*, RefLock*> PlanetLockMap;

		RefLock m_account_manager_lock;
		RefLock m_world_lock;

		mutable boost::mutex m_planet_locks_mutex;
		PlanetLockMap m_planet_locks;
};

}
