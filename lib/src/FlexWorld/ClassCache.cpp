#include <FlexWorld/ClassCache.hpp>

#include <cassert>
#include <limits>

namespace fw {

ClassCache::ClassCache() :
	m_num_holes( 0 )
{
}

void ClassCache::clear() {
	m_classes.clear();
	m_ids.clear();
	m_num_holes = 0;
}

std::size_t ClassCache::get_num_cached_classes() const {
	return m_ids.size();
}

const Class& ClassCache::get_class( IdType id ) const {
	assert( is_id_valid( id ) );
	return *m_classes[id];
}

ClassCache::IdType ClassCache::cache( const Class& cls ) {
	// Check if same class has already been cached.
	IdUsePairMap::iterator iu_iter( m_ids.find( &cls ) );

	if( iu_iter != m_ids.end() ) {
		// Increase usage counter.
		++iu_iter->second.second;
		return iu_iter->second.first;
	}

	// If no holes, just cache.
	if( !m_num_holes ) {
		m_classes.push_back( &cls );
		m_ids[&cls] = IdUsePair( m_classes.size() - 1, 1 );
		return static_cast<IdType>( m_classes.size() - 1 );
	}

	// Hole exists, find it.
	for( std::size_t index = 0; index < m_classes.size(); ++index ) {
		if( m_classes[index] == nullptr ) {
			--m_num_holes;
			m_classes[index] = &cls;
			m_ids[&cls] = IdUsePair( index, 1 );
			return static_cast<IdType>( index );
		}
	}

	// Hole not found, this really shouldn't happen.
	return 0;
}

void ClassCache::forget( const Class& cls ) {
	IdUsePairMap::iterator iu_iter( m_ids.find( &cls ) );

	assert( iu_iter != m_ids.end() );

	--iu_iter->second.second;
	if( iu_iter->second.second > 0 ) {
		// Still in use, cancel removing.
		return;
	}

	// If last cached class, no hole.
	if( iu_iter->second.first == m_classes.size() - 1 ) {
		m_classes.pop_back();
		m_ids.erase( iu_iter );
		return;
	}

	// Hole!
	m_classes[iu_iter->second.first] = nullptr;
	++m_num_holes;
	m_ids.erase( iu_iter );
}

uint32_t ClassCache::get_num_holes() const {
	return m_num_holes;
}

bool ClassCache::is_id_valid( IdType id ) const {
	if( id >= m_classes.size() ) {
		return false;
	}

	return m_classes[id] != nullptr;
}

}
