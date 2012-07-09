#include <FlexWorld/RefLock.hpp>

namespace fw {

RefLock::RefLock() :
	m_usage_count( 0 )
{
}

std::size_t RefLock::get_usage_count() const {
	std::size_t usage_count = 0;

	{
		boost::lock_guard<boost::mutex> acc_lock( m_access_mutex );
		usage_count = m_usage_count;
	}

	return usage_count;
}

void RefLock::lock() {
	{
		boost::lock_guard<boost::mutex> acc_lock( m_access_mutex );

		// If same thread has already acquired the lock, just increase the usage
		// counter.
		if( m_current_thread == boost::this_thread::get_id() ) {
			++m_usage_count;
			return;
		}
	}

	// This thread has not acquired, so do it now.
	m_lock_mutex.lock();

	// Lock succeeded, update info.
	{
		boost::lock_guard<boost::mutex> acc_lock( m_access_mutex );

		assert( m_usage_count == 0 );
		assert( m_current_thread != boost::this_thread::get_id() );

		++m_usage_count;
		m_current_thread = boost::this_thread::get_id();
	}
}

void RefLock::unlock() {
	{
		boost::lock_guard<boost::mutex> acc_lock( m_access_mutex );

		assert( m_usage_count > 0 );
		assert( m_current_thread == boost::this_thread::get_id() );

		--m_usage_count;

		if( m_usage_count == 0 ) {
			m_current_thread = boost::thread::id();
			m_lock_mutex.unlock();
		}
	}
}

}
