#include <FlexWorld/LockFacility.hpp>

#include <iostream>

namespace fw {

LockFacility::LockFacility() :
	m_num_locked_planets( 0 )
{
}

LockFacility::~LockFacility() {
	boost::lock_guard<boost::mutex> lock( m_internal_lock );

#if !defined( NDEBUG )
	// Warn if there're still planet locks or even planets locked.
	if( m_num_locked_planets > 0 ) {
		std::cout << "*** WARNING *** Planets still locked." << std::endl;
	}
#endif

	if( m_planet_locks.size() > 0 ) {
#if !defined( NDEBUG )
		std::cout << "*** WARNING *** Planet locks still available." << std::endl;
#endif

		PlanetLockMap::iterator lock_iter( m_planet_locks.begin() );
		PlanetLockMap::iterator lock_iter_end( m_planet_locks.end() );
		
		for( ; lock_iter != lock_iter_end; ++lock_iter ) {
			delete lock_iter->second;
		}
	}

}

void LockFacility::lock_account_manager( bool do_lock ) {
	if( do_lock ) {
		m_account_manager_lock.lock();
	}
	else {
		assert( m_account_manager_lock.get_usage_count() > 0 );
		m_account_manager_lock.unlock();
	}
}

bool LockFacility::is_account_manager_locked() const {
	return m_account_manager_lock.get_usage_count() > 0;
}

void LockFacility::lock_world( bool do_lock ) {
	if( do_lock ) {
		m_world_lock.lock();
	}
	else {
		assert( m_world_lock.get_usage_count() > 0 );
		m_world_lock.unlock();
	}
}

bool LockFacility::is_world_locked() const {
	return m_world_lock.get_usage_count() > 0;
}

void LockFacility::lock_planet( const Planet& planet, bool do_lock ) {
	RefLock* ref_lock = nullptr;

	// Get RefLock ptr.
	{
		boost::lock_guard<boost::mutex> lock( m_internal_lock );

		PlanetLockMap::iterator iter = m_planet_locks.find( &planet );
	
		assert( iter != m_planet_locks.end() );
		if( iter == m_planet_locks.end() ) {
			return;
		}

		ref_lock = iter->second;
	}

	// Lock/unlock.
	if( do_lock ) {
		ref_lock->lock();
	}
	else {
		assert( ref_lock->get_usage_count() > 0 );
		ref_lock->unlock();
	}

	// Update planet lock counter.
	{
		boost::lock_guard<boost::mutex> lock( m_internal_lock );

		if( do_lock ) {
			++m_num_locked_planets;
		}
		else {
			--m_num_locked_planets;
		}
	}
}

bool LockFacility::is_planet_locked( const Planet& planet ) const {
	bool is_locked = false;

	{
		boost::lock_guard<boost::mutex> lock( m_internal_lock );

		PlanetLockMap::const_iterator iter = m_planet_locks.find( &planet );
		assert( iter != m_planet_locks.end() );

		if( iter != m_planet_locks.end() ) {
			is_locked = iter->second->get_usage_count() > 0;
		}
	}

	return is_locked;
}

std::size_t LockFacility::get_num_planet_locks() const {
	std::size_t num = 0;

	{
		boost::lock_guard<boost::mutex> lock( m_internal_lock );
		num = m_planet_locks.size();
	}

	return num;
}

std::size_t LockFacility::get_num_locked_planets() const {
	std::size_t num = 0;

	{
		boost::lock_guard<boost::mutex> lock( m_internal_lock );
		num = m_num_locked_planets;
	}

	return num;
}

void LockFacility::create_planet_lock( const Planet& planet ) {
	boost::lock_guard<boost::mutex> lock( m_internal_lock );

	assert( m_planet_locks.find( &planet ) == m_planet_locks.end() );

	m_planet_locks[&planet] = new RefLock;
}

void LockFacility::destroy_planet_lock( const Planet& planet ) {
	assert( is_planet_locked( planet ) == false );

	boost::lock_guard<boost::mutex> lock( m_internal_lock );

	PlanetLockMap::iterator iter = m_planet_locks.find( &planet );
	assert( iter != m_planet_locks.end() );

	if( iter != m_planet_locks.end() ) {
		if( iter->second->get_usage_count() == 0 ) {
			delete iter->second;
			m_planet_locks.erase( iter );
		}
		else {
#if !defined( NDEBUG )
			std::cout << "*** WARNING *** Tried to destroy a locked planet." << std::endl;
#endif
		}
	}
}

}
