#pragma once

#include <FlexWorld/RefLock.hpp>

#include <map>

namespace fw {

class Planet;

/** Lock facility for securing several backend objects.
 *
 * Uses RefLock objects internally, i.e. it's safe to do multiple locks from
 * the same thread. Do not forget to unlock! ;-)
 */
class LockFacility {
	public:
		/** Ctor.
		 */
		LockFacility();

		/** Dtor.
		 */
		~LockFacility();

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
		 * @param planet Planet (lock must have been created before).
		 * @param do_lock true to lock, false to unlock.
		 * @see create_planet_lock
		 */
		void lock_planet( const Planet& planet, bool do_lock );

		/** Check if planet is locked.
		 * @param planet Planet (lock must have been created before).
		 * @return true when locked.
		 * @see create_planet_lock
		 */
		bool is_planet_locked( const Planet& planet ) const;

		/** Get number of planet locks.
		 * @return Number of planet locks.
		 */
		std::size_t get_num_planet_locks() const;

		/** Get number of currently locked planets.
		 * @return Number of locked planets.
		 */
		std::size_t get_num_locked_planets() const;

		/** Create planet lock.
		 * @param planet Planet.
		 */
		void create_planet_lock( const Planet& planet );

		/** Destroy planet lock.
		 * @param planet Planet.
		 */
		void destroy_planet_lock( const Planet& planet );

	private:
		typedef std::map<const Planet*, RefLock*> PlanetLockMap;

		PlanetLockMap m_planet_locks;

		RefLock m_account_manager_lock;
		RefLock m_world_lock;

		mutable boost::mutex m_internal_lock;

		std::size_t m_num_locked_planets;
};

}
