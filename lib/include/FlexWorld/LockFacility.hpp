#pragma once

#include <boost/thread.hpp>

namespace flex {

/** Lock facility for securing several backend objects.
 *
 * The lock facility implements an usage count strategy per thread. That means
 * if the same thread (ID) is requesting a lock (again), the mutex usage
 * counter is increased but not locked again to prevent deadlocks.
 *
 * As soon as the usage counter reaches zero (by unlocking) the mutex is
 * unlocked and other requesters can lock it.
 */
class LockFacility {
	public:
		/** Ctor.
		 */
		LockFacility();

		/** Lock or unlock account manager.
		 * @param do_lock true to lock, false to unlock.
		 */
		void lock_account_manager( bool do_lock );

		/** Check if account manager is locked.
		 * @return true when locked.
		 */
		bool is_account_manager_locked() const;

	private:
		struct LockInfo {
			LockInfo();

			mutable boost::mutex access_mutex;
			boost::mutex lock_mutex;
			boost::thread::id last_thread;
			std::size_t usage_count;
		};

		LockInfo m_account_manager_lock;
};

}
