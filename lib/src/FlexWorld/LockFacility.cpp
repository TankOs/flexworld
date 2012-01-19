#include <FlexWorld/LockFacility.hpp>

namespace flex {

LockFacility::LockInfo::LockInfo() :
	usage_count( 0 )
{
}

LockFacility::LockFacility() {
}

void LockFacility::lock_account_manager( bool do_lock ) {
	if( do_lock ) {
		{
			boost::lock_guard<boost::mutex> guard( m_account_manager_lock.access_mutex );

			// At first check if the same thread is trying to lock again.
			if( m_account_manager_lock.last_thread == boost::this_thread::get_id() ) {
				++m_account_manager_lock.usage_count;
				return;
			}
		}

		// At this point the lock has been acquired by another thread. Wait for it to
		// become available.
		m_account_manager_lock.lock_mutex.lock();

		// Store info.
		{
			boost::lock_guard<boost::mutex> guard( m_account_manager_lock.access_mutex );
			
			++m_account_manager_lock.usage_count;
			m_account_manager_lock.last_thread = boost::this_thread::get_id();
		}
	}
	else {
		{
			boost::lock_guard<boost::mutex> guard( m_account_manager_lock.access_mutex );

			assert( m_account_manager_lock.last_thread == boost::this_thread::get_id() );
			assert( m_account_manager_lock.usage_count > 0 );

			--m_account_manager_lock.usage_count;

			// If not more in use, release.
			if( m_account_manager_lock.usage_count == 0 ) {
				m_account_manager_lock.last_thread = boost::thread::id();
				m_account_manager_lock.lock_mutex.unlock();
			}
		}
	}
}

bool LockFacility::is_account_manager_locked() const {
	bool is_locked = false;

	{
		boost::lock_guard<boost::mutex> lock( m_account_manager_lock.access_mutex );
		is_locked = (m_account_manager_lock.usage_count > 0);
	}

	return is_locked;
}

}
