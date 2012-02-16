#include <FlexWorld/LockFacility.hpp>

namespace flex {

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
	boost::lock_guard<boost::mutex> map_lock( m_planet_locks_mutex );

	if( do_lock ) {
		PlanetLockMap::iterator lock_iter = m_planet_locks.find( &planet );

		if( lock_iter == m_planet_locks.end() ) {
			lock_iter = m_planet_locks.insert( std::pair<const Planet*, RefLock*>( &planet, new RefLock ) ).first;
		}

		lock_iter->second->lock();
	}
	else {
		PlanetLockMap::iterator lock_iter = m_planet_locks.find( &planet );
		assert( lock_iter != m_planet_locks.end() );

		lock_iter->second->unlock();

		if( lock_iter->second->get_usage_count() == 0 ) {
			delete lock_iter->second;
			m_planet_locks.erase( lock_iter );
		}
	}
}

bool LockFacility::is_planet_locked( const Planet& planet ) const {
	bool is_locked = false;

	{
		boost::lock_guard<boost::mutex> map_lock( m_planet_locks_mutex );
		is_locked = m_planet_locks.find( &planet ) != m_planet_locks.end();
	}

	return is_locked;
}

void LockFacility::lock_entity( const Entity& entity, bool do_lock ) {
	boost::lock_guard<boost::mutex> map_lock( m_entity_locks_mutex );

	if( do_lock ) {
		EntityLockMap::iterator lock_iter = m_entity_locks.find( &entity );

		if( lock_iter == m_entity_locks.end() ) {
			lock_iter = m_entity_locks.insert( std::pair<const Entity*, RefLock*>( &entity, new RefLock ) ).first;
		}

		lock_iter->second->lock();
	}
	else {
		EntityLockMap::iterator lock_iter = m_entity_locks.find( &entity );
		assert( lock_iter != m_entity_locks.end() );

		lock_iter->second->unlock();

		if( lock_iter->second->get_usage_count() == 0 ) {
			delete lock_iter->second;
			m_entity_locks.erase( lock_iter );
		}
	}
}

bool LockFacility::is_entity_locked( const Entity& entity ) const {
	bool is_locked = false;

	{
		boost::lock_guard<boost::mutex> map_lock( m_entity_locks_mutex );
		is_locked = m_entity_locks.find( &entity ) != m_entity_locks.end();
	}

	return is_locked;
}

}
