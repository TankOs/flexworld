#include <FlexWorld/ClassCache.hpp>

namespace flex {

ClassCache::ClassCache() :
	m_num_holes( 0 )
{
}

std::size_t ClassCache::get_size() const {
	return m_ids.size();
}

const Class* ClassCache::get_class( IdType id ) const {
	if( id == 0 || id > m_classes.size() ) {
		return nullptr;
	}

	return m_classes[id - 1];
}

ClassCache::IdType ClassCache::cache( const Class& cls ) {
	// Check if same class has already been cached.
	IdUsePairMap::iterator iu_iter( m_ids.find( &cls ) );

	if( iu_iter != m_ids.end() ) {
		++iu_iter->second.second;
		return iu_iter->second.first;
	}

	// If no holes, just cache.
	if( !m_num_holes ) {
		m_classes.push_back( &cls );
		m_ids[&cls] = IdUsePair( m_classes.size(), 1 );
		return static_cast<IdType>( m_classes.size() );
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

std::size_t ClassCache::get_use_count( const Class& cls ) const {
	IdUsePairMap::const_iterator iu_iter( m_ids.find( &cls ) );

	if( iu_iter == m_ids.end() ) {
		return 0;
	}

	return iu_iter->second.second;
}

void ClassCache::forget( const Class& cls ) {
	IdUsePairMap::iterator iu_iter( m_ids.find( &cls ) );

	if( iu_iter == m_ids.end() ) {
		return;
	}

	--iu_iter->second.second;
	if( iu_iter->second.second > 0 ) {
		// Still in use, cancel removing.
		return;
	}

	// If last cached class, no hole.
	if( iu_iter->second.first == m_classes.size() ) {
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

}
