#include <FlexWorld/LockFacility.hpp>

namespace flex {

void LockFacility::lock_account_manager( bool do_lock ) {
	if( do_lock ) {
		m_account_manager_lock.lock();
	}
	else {
		m_account_manager_lock.unlock();
	}
}

bool LockFacility::is_account_manager_locked() const {
	return m_account_manager_lock.get_usage_count() > 0;
}

}
