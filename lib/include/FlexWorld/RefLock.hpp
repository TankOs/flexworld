#pragma once

#include <boost/thread.hpp>

namespace flex {

/** Specialized reference-counted lock.
 *
 * RefLocks take care of who locked and how many times. Each time the lock is
 * acquired by the same thread, an usage counter is increased and a re-lock is
 * not done to avoid deadlocks. As soon as the usage counter reaches zero (by
 * unlocking), the lock is released.
 *
 * Every call to RefLock is thread-safe.
 */
class RefLock {
	public:
		/** Ctor.
		 */
		RefLock();

		/** Get usage count.
		 * @return Usage count (0 = not locked).
		 */
		std::size_t get_usage_count() const;

		/** Lock.
		 */
		void lock();

		/** Unlock.
		 */
		void unlock();

	private:
		mutable boost::mutex m_access_mutex;
		boost::mutex m_lock_mutex;
		boost::thread::id m_current_thread;

		std::size_t m_usage_count;
};

}
