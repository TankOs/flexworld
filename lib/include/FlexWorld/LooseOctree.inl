#include <cassert>
#include <cstring>
#include <iostream> // XXX 

namespace flex {

template <class T, class DVS>
LooseOctree<T, DVS>::LooseOctree( Size size ) :
	m_position( 0, 0, 0 ),
	m_data( nullptr ),
	m_children( nullptr ),
	m_size( size )
{
}

template <class T, class DVS>
LooseOctree<T, DVS>::LooseOctree( const Vector& position, Size size ) :
	m_position( position ),
	m_data( nullptr ),
	m_children( nullptr ),
	m_size( size )
{
}

template <class T, class DVS>
LooseOctree<T, DVS>::~LooseOctree() {
	delete m_data;

	if( m_children ) {
		for( std::size_t child_idx = 0; child_idx < 8; ++child_idx ) {
			delete m_children[child_idx];
		}
	}

	delete[] m_children;
}

template <class T, class DVS>
typename LooseOctree<T, DVS>::Size LooseOctree<T, DVS>::get_size() const {
	return m_size;
}

template <class T, class DVS>
const typename LooseOctree<T, DVS>::Vector& LooseOctree<T, DVS>::get_position() const {
	return m_position;
}

template <class T, class DVS>
bool LooseOctree<T, DVS>::is_subdivided() const {
	return m_children != nullptr;
}

template <class T, class DVS>
std::size_t LooseOctree<T, DVS>::get_num_data() const {
	return m_data != nullptr ? m_data->size() : 0;
}

template <class T, class DVS>
void LooseOctree<T, DVS>::ensure_data() {
	if( !m_data ) {
		m_data = new DataList;
	}
}

template <class T, class DVS>
LooseOctree<T, DVS>& LooseOctree<T, DVS>::insert( const T& data, const typename DataInfo::Cuboid& cuboid ) {
	assert( cuboid.width <= m_size );
	assert( cuboid.height <= m_size );
	assert( cuboid.depth <= m_size );

	assert( cuboid.x >= m_position.x );
	assert( cuboid.y >= m_position.y );
	assert( cuboid.z >= m_position.z );

	assert( cuboid.x + cuboid.width <= m_position.x + m_size );
	assert( cuboid.y + cuboid.height <= m_position.y + m_size );
	assert( cuboid.z + cuboid.depth <= m_position.z + m_size );

	// Determine which quadrant the data belongs to.
	Quadrant quadrant = determine_quadrant( cuboid );
	assert( quadrant != INVALID_QUADRANT );

	// If same quadrant, just add data to list.
	if( quadrant == SAME_QUADRANT ) {
		ensure_data();

		// Push back first so data doesn't get copied 2 times.
		m_data->push_back( DataInfo() );

		m_data->back().cuboid = cuboid;
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
	return m_children[quadrant]->insert( data, cuboid );
}

template <class T, class DVS>
void LooseOctree<T, DVS>::subdivide() {
	assert( !is_subdivided() );
	assert( m_size > 1 );

	m_children = new LooseOctree<T, DVS>*[8];
	std::memset( m_children, 0, 8 * sizeof( LooseOctree<T, DVS>* ) );
}

template <class T, class DVS>
void LooseOctree<T, DVS>::create_child( Quadrant quadrant ) {
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

	m_children[quadrant] = new LooseOctree<T, DVS>( position, size );
}

template <class T, class DVS>
typename LooseOctree<T, DVS>::Quadrant LooseOctree<T, DVS>::determine_quadrant( const typename DataInfo::Cuboid& cuboid ) {
	assert( cuboid.width <= m_size );
	assert( cuboid.height <= m_size );
	assert( cuboid.depth <= m_size );

	// Calculate center point.
	typename DataInfo::Vector center(
		cuboid.x + cuboid.width / 2,
		cuboid.y + cuboid.height / 2,
		cuboid.z + cuboid.depth / 2
	);

	assert( center.x >= m_position.x );
	assert( center.y >= m_position.y );
	assert( center.z >= m_position.z );
	assert( center.x <= m_position.x + m_size );
	assert( center.y <= m_position.y + m_size );
	assert( center.z <= m_position.z + m_size );

	// If size is higher than half of any dimension, it belongs to the same node.
	if(
		cuboid.width > m_size / 2 ||
		cuboid.height > m_size / 2 ||
		cuboid.depth > m_size / 2
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

template <class T, class DVS>
bool LooseOctree<T, DVS>::has_child( Quadrant quadrant ) const {
	assert( quadrant < SAME_QUADRANT );

	if( !is_subdivided() ) {
		return false;
	}

	return m_children[quadrant] != nullptr;
}

template <class T, class DVS>
LooseOctree<T, DVS>& LooseOctree<T, DVS>::get_child( Quadrant quadrant ) const {
	assert( quadrant < SAME_QUADRANT );
	assert( has_child( quadrant ) );

	return *m_children[quadrant];
}

template <class T, class DVS>
const typename LooseOctree<T, DVS>::DataList& LooseOctree<T, DVS>::get_data() const {
	assert( m_data != nullptr );

	return *m_data;
}

template <class T, class DVS>
void LooseOctree<T, DVS>::search( const typename DataInfo::Cuboid& cuboid, ResultArray& /*results*/ ) const {
	assert( cuboid.x >= m_position.x );
	assert( cuboid.y >= m_position.y );
	assert( cuboid.z >= m_position.z );
	assert( cuboid.x + cuboid.width <= m_position.x + m_size );
	assert( cuboid.y + cuboid.height <= m_position.y + m_size );
	assert( cuboid.z + cuboid.depth <= m_position.z + m_size );

	// If this node contains data, check for collision.
	if( m_data && m_data->size() > 0 ) {
		typename DataList::const_iterator data_iter( (*m_data).begin() );
		typename DataList::const_iterator data_iter_end( (*m_data).end() );
		
		// Check each data entry for collision with the cuboid.
		for( ; data_iter != data_iter_end; ++data_iter ) {
			//const DataInfo& info = *data_iter;
		}
	}
}

///// DataInfo //////

template <class T, class DVS>
LooseOctree<T, DVS>::DataInfo::DataInfo() :
	cuboid( 0, 0, 0, 0, 0, 0 )
{
}

}
