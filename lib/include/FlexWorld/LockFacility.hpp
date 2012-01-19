#pragma once

#include <FlexWorld/RefLock.hpp>

namespace flex {

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

	private:
		RefLock m_account_manager_lock;
};

}
