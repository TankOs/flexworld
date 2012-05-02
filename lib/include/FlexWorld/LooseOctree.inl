#include <cassert>
#include <cstring>

namespace flex {

template <class DT>
LooseOctree<DT>::LooseOctree( Size size ) :
	m_position( 0, 0, 0 ),
	m_data( nullptr ),
	m_children( nullptr ),
	m_size( size )
{
}

template <class DT>
LooseOctree<DT>::LooseOctree( const Vector& position, Size size ) :
	m_position( position ),
	m_data( nullptr ),
	m_children( nullptr ),
	m_size( size )
{
}

template <class DT>
LooseOctree<DT>::~LooseOctree() {
	delete m_data;

	if( m_children ) {
		for( std::size_t child_idx = 0; child_idx < 8; ++child_idx ) {
			delete m_children[child_idx];
		}
	}

	delete[] m_children;
}

template <class DT>
typename LooseOctree<DT>::Size LooseOctree<DT>::get_size() const {
	return m_size;
}

template <class DT>
const typename LooseOctree<DT>::Vector& LooseOctree<DT>::get_position() const {
	return m_position;
}

template <class DT>
bool LooseOctree<DT>::is_subdivided() const {
	return m_children != nullptr;
}

template <class DT>
std::size_t LooseOctree<DT>::get_num_data() const {
	return m_data != nullptr ? m_data->size() : 0;
}

template <class DT>
void LooseOctree<DT>::ensure_data() {
	if( !m_data ) {
		m_data = new DataList;
	}
}

template <class DT>
LooseOctree<DT>& LooseOctree<DT>::insert( const DT& data, const Vector& center, Size size ) {
	assert( size <= m_size );
	assert( center.x >= m_position.x );
	assert( center.y >= m_position.y );
	assert( center.z >= m_position.z );
	assert( center.x < m_position.x + m_size );
	assert( center.y < m_position.y + m_size );
	assert( center.z < m_position.z + m_size );

	// Determine which quadrant the data belongs to.
	Quadrant quadrant = determine_quadrant( center, size );
	assert( quadrant != INVALID_QUADRANT );

	// If same quadrant, just add data to list.
	if( quadrant == SAME_QUADRANT ) {
		ensure_data();

		// Push back first so data doesn't get copied 2 times.
		m_data->push_back( DataInfo() );

		m_data->back().position = center;
		m_data->back().size = size;
		m_data->back().data = data;

		return *this;
	}

	// Subdivide if needed.
	if( !is_subdivided() ) {
		subdivide();
	}

	// Create child node if needed.
	if( m_children[quadrant] == nullptr ) {
		create_child( quadrant );
	}

	// Insert data at child.
	return m_children[quadrant]->insert( data, center, size );
}

template <class DT>
void LooseOctree<DT>::subdivide() {
	assert( !is_subdivided() );
	assert( m_size > 1 );

	m_children = new LooseOctree<DT>*[8];
	std::memset( m_children, 0, 8 * sizeof( LooseOctree<DT>* ) );
}

template <class DT>
void LooseOctree<DT>::create_child( Quadrant quadrant ) {
	assert( quadrant < SAME_QUADRANT );
	assert( is_subdivided() );
	assert( m_size > 1 );
	assert( m_children[quadrant] == nullptr );

	Vector position = m_position;
	Size size = m_size / 2;

	switch( quadrant ) {
		case LEFT_BOTTOM_FAR:
			break;

		case RIGHT_BOTTOM_FAR:
			position.x += size;
			break;

		case LEFT_BOTTOM_NEAR:
			position.z += size;
			break;

		case RIGHT_BOTTOM_NEAR:
			position.x += size;
			position.z += size;
			break;

		case LEFT_TOP_FAR:
			position.y += size;
			break;

		case RIGHT_TOP_FAR:
			position.x += size;
			position.y += size;
			break;

		case LEFT_TOP_NEAR:
			position.z += size;
			position.y += size;
			break;

		case RIGHT_TOP_NEAR:
			position.x += size;
			position.y += size;
			position.z += size;
			break;

		default:
			assert( 0 && "Invalid quadrant." );
			break;
	}

	m_children[quadrant] = new LooseOctree<DT>( position, size );
}

template <class DT>
typename LooseOctree<DT>::Quadrant LooseOctree<DT>::determine_quadrant( const Vector& center, Size size ) {
	assert( size <= m_size );
	assert( center.x >= m_position.x );
	assert( center.y >= m_position.y );
	assert( center.z >= m_position.z );
	assert( center.x < m_position.x + m_size );
	assert( center.y < m_position.y + m_size );
	assert( center.z < m_position.z + m_size );

	// If size is higher than half of any dimension, it belongs to the same node.
	if(
		size > m_size / 2 ||
		size > m_size / 2 ||
		size > m_size / 2
	) {
		return SAME_QUADRANT;
	}

	Size half_x = m_position.x + m_size / 2;
	Size half_y = m_position.y + m_size / 2;
	Size half_z = m_position.z + m_size / 2;

	if( center.x < half_x ) { // Left quadrants.
		if( center.y < half_y ) { // Bottom quadrants.
			if( center.z < half_z ) { // Far quadrants.
				return LEFT_BOTTOM_FAR;
			}
			else { // Near quadrants.
				return LEFT_BOTTOM_NEAR;
			}
		}
		else { // Top quadrants.
			if( center.z < half_z ) { // Far quadrants.
				return LEFT_TOP_FAR;
			}
			else { // Near quadrants.
				return LEFT_TOP_NEAR;
			}
		}
	}
	else { // Right quadrants.
		if( center.y < half_y ) { // Bottom quadrants.
			if( center.z < half_z ) { // Far quadrants.
				return RIGHT_BOTTOM_FAR;
			}
			else { // Near quadrants.
				return RIGHT_BOTTOM_NEAR;
			}
		}
		else { // Top quadrants.
			if( center.z < half_z ) { // Far quadrants.
				return RIGHT_TOP_FAR;
			}
			else { // Near quadrants.
				return RIGHT_TOP_NEAR;
			}
		}
	}

	return INVALID_QUADRANT;
}

template <class DT>
bool LooseOctree<DT>::has_child( Quadrant quadrant ) const {
	assert( quadrant < SAME_QUADRANT );

	if( !is_subdivided() ) {
		return false;
	}

	return m_children[quadrant] != nullptr;
}

template <class DT>
LooseOctree<DT>& LooseOctree<DT>::get_child( Quadrant quadrant ) const {
	assert( quadrant < SAME_QUADRANT );
	assert( has_child( quadrant ) );

	return *m_children[quadrant];
}

template <class DT>
const typename LooseOctree<DT>::DataList& LooseOctree<DT>::get_data() const {
	assert( m_data != nullptr );

	return *m_data;
}

///// DataInfo //////

template <class DT>
LooseOctree<DT>::DataInfo::DataInfo() :
	position( 0, 0, 0 ),
	size( 1 )
{
}

}
