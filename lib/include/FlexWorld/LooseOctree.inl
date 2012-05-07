#include <cassert>
#include <cstring>

namespace flex {

template <class T, class DVS>
inline void continue_search(
	const LooseOctree<T, DVS>* child,
	const typename LooseOctree<T, DVS>::DataCuboid& cuboid,
	typename LooseOctree<T, DVS>::DataArray& results
) {
	if( !child ) {
		return;
	}

	if(
		LooseOctree<T, DVS>::DataCuboid::calc_intersection(
			typename LooseOctree<T, DVS>::DataCuboid(
				(
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_position().x ) -
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() / 2 )
				),
				(
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_position().y ) -
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() / 2 )
				),
				(
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_position().z ) -
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() / 2 )
				),
				static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() * 2 ),
				static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() * 2 ),
				static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() * 2 )
			),
			cuboid
		).width > 0
	) {
		child->search( cuboid, results );
	}
}

template <class T, class DVS>
inline void continue_erase(
	LooseOctree<T, DVS>* child,
	const T& data,
	const typename LooseOctree<T, DVS>::DataCuboid& cuboid
) {
	if( !child ) {
		return;
	}

	if(
		LooseOctree<T, DVS>::DataCuboid::calc_intersection(
			typename LooseOctree<T, DVS>::DataCuboid(
				(
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_position().x ) -
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() / 2 )
				),
				(
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_position().y ) -
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() / 2 )
				),
				(
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_position().z ) -
					static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() / 2 )
				),
				static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() * 2 ),
				static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() * 2 ),
				static_cast<typename LooseOctree<T, DVS>::DataCuboid::Type>( child->get_size() * 2 )
			),
			cuboid
		).width > 0
	) {
		child->erase( data, cuboid );
	}
}

template <class T, class DVS>
LooseOctree<T, DVS>::LooseOctree( Size size ) :
	m_position( 0, 0, 0 ),
	m_data( nullptr ),
	m_parent( nullptr ),
	m_children( nullptr ),
	m_size( size )
{
}

template <class T, class DVS>
LooseOctree<T, DVS>::LooseOctree( const Vector& position, Size size, LooseOctree<T, DVS>* parent ) :
	m_position( position ),
	m_data( nullptr ),
	m_parent( parent ),
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
LooseOctree<T, DVS>& LooseOctree<T, DVS>::insert( const T& data, const DataCuboid& cuboid ) {
#if !defined( NDEBUG )
	FloatCuboid node_cuboid(
		static_cast<float>( m_position.x ) - (static_cast<float>( m_size ) / 2.0f),
		static_cast<float>( m_position.y ) - (static_cast<float>( m_size ) / 2.0f),
		static_cast<float>( m_position.z ) - (static_cast<float>( m_size ) / 2.0f),
		static_cast<float>( m_position.x ) + (static_cast<float>( m_size ) * 2.0f),
		static_cast<float>( m_position.y ) + (static_cast<float>( m_size ) * 2.0f),
		static_cast<float>( m_position.z ) + (static_cast<float>( m_size ) * 2.0f)
	);

	assert( cuboid.width <= m_size );
	assert( cuboid.height <= m_size );
	assert( cuboid.depth <= m_size );

	assert( cuboid.x >= node_cuboid.x );
	assert( cuboid.y >= node_cuboid.y );
	assert( cuboid.z >= node_cuboid.z );

	assert( cuboid.x + cuboid.width <= node_cuboid.x + node_cuboid.width );
	assert( cuboid.y + cuboid.height <= node_cuboid.y + node_cuboid.height );
	assert( cuboid.z + cuboid.depth <= node_cuboid.z + node_cuboid.depth );
#endif

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

	m_children[quadrant] = new LooseOctree<T, DVS>( position, size, this );
}

template <class T, class DVS>
typename LooseOctree<T, DVS>::Quadrant LooseOctree<T, DVS>::determine_quadrant( const DataCuboid& cuboid ) {
	assert( cuboid.width <= m_size );
	assert( cuboid.height <= m_size );
	assert( cuboid.depth <= m_size );

	// Calculate center point.
	DataVector center(
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
typename LooseOctree<T, DVS>::DataArray LooseOctree<T, DVS>::get_data() const {
	if( !m_data ) {
		return DataArray();
	}

	DataArray data;

	typename DataList::const_iterator data_iter( m_data->begin() );
	typename DataList::const_iterator data_iter_end( m_data->end() );
	
	for( ; data_iter != data_iter_end; ++data_iter ) {
		data.push_back( data_iter->data );
	}

	return data;
}

template <class T, class DVS>
void LooseOctree<T, DVS>::search( const DataCuboid& cuboid, DataArray& results ) const {
	// No checks for cuboid needed here, as we're testing for intersections anyways.

	// If this node contains data, check for collision.
	if( m_data && m_data->size() > 0 ) {
		typename DataList::const_iterator data_iter( (*m_data).begin() );
		typename DataList::const_iterator data_iter_end( (*m_data).end() );
		
		// Check each data entry for collision with the cuboid.
		for( ; data_iter != data_iter_end; ++data_iter ) {
			const DataInfo& info = *data_iter;
			DataCuboid intersection = DataCuboid::calc_intersection( info.cuboid, cuboid );

			if(
				intersection.width > 0 &&
				intersection.height > 0 &&
				intersection.depth > 0
			) {
				results.push_back( info.data );
			}
		}
	}

	// Find out which children the cuboid intersects with. Continue searching for
	// data in those nodes.
	if( !m_children ) {
		// No children, nothing to do.
		return;
	}

	continue_search( m_children[LEFT_BOTTOM_FAR], cuboid, results );
	continue_search( m_children[RIGHT_BOTTOM_FAR], cuboid, results );
	continue_search( m_children[LEFT_BOTTOM_NEAR], cuboid, results );
	continue_search( m_children[RIGHT_BOTTOM_NEAR], cuboid, results );
	continue_search( m_children[LEFT_TOP_FAR], cuboid, results );
	continue_search( m_children[RIGHT_TOP_FAR], cuboid, results );
	continue_search( m_children[LEFT_TOP_NEAR], cuboid, results );
	continue_search( m_children[RIGHT_TOP_NEAR], cuboid, results );
}

///// DataInfo //////

template <class T, class DVS>
LooseOctree<T, DVS>::DataInfo::DataInfo() :
	cuboid( 0, 0, 0, 0, 0, 0 )
{
}

template <class T, class DVS>
void LooseOctree<T, DVS>::erase( const T& data, const DataCuboid& cuboid ) {
	// Traverse to children at first.
	if( m_children ) {
		continue_erase( m_children[LEFT_BOTTOM_FAR], data, cuboid );
		continue_erase( m_children[RIGHT_BOTTOM_FAR], data, cuboid );
		continue_erase( m_children[LEFT_BOTTOM_NEAR], data, cuboid );
		continue_erase( m_children[RIGHT_BOTTOM_NEAR], data, cuboid );
		continue_erase( m_children[LEFT_TOP_FAR], data, cuboid );
		continue_erase( m_children[RIGHT_TOP_FAR], data, cuboid );
		continue_erase( m_children[LEFT_TOP_NEAR], data, cuboid );
		continue_erase( m_children[RIGHT_TOP_NEAR], data, cuboid );
	}

	// If this node contains data, check for collision.
	if( m_data && m_data->size() > 0 ) {
		typename DataList::iterator data_iter( m_data->begin() );
		
		// Check each data entry for collision with the cuboid.
		while( data_iter != m_data->end() ) {
			const DataInfo& info = *data_iter;
			DataCuboid intersection = DataCuboid::calc_intersection( info.cuboid, cuboid );

			if(
				intersection.width > 0 &&
				intersection.height > 0 &&
				intersection.depth > 0 &&
				info.data == data
			) {
				// Hit, erase.
				data_iter = m_data->erase( data_iter );
			}
			else {
				++data_iter;
			}
		}
	}

	cleanup( false );
}

template <class T, class DVS>
void LooseOctree<T, DVS>::erase( const T& data ) {
	if( !m_data || m_data->size() < 1 ) {
		return;
	}

	typename DataList::iterator data_iter( m_data->begin() );
	
	// Check each data entry if it's the one being requested to be erased.
	while( data_iter != m_data->end() ) {
		if( data_iter->data == data ) {
			// Hit, erase.
			data_iter = m_data->erase( data_iter );
		}
		else {
			++data_iter;
		}
	}

	cleanup( true );
}

template <class T, class DVS>
void LooseOctree<T, DVS>::cleanup( bool recursive ) {
	if( m_children ) {
		// Check each child if it isn't subdivided and doesn't hold data anymore, so
		// it can be destroyed.
		std::size_t num_empty_children = 0;
		std::size_t num_total_children = 0;

		for( std::size_t child_idx = 0; child_idx < SAME_QUADRANT; ++child_idx ) {
			if( m_children[child_idx] ) {
				++num_total_children;

				if(
					!m_children[child_idx]->is_subdivided() &&
					!m_children[child_idx]->get_num_data()
				) {
					delete m_children[child_idx];
					m_children[child_idx] = nullptr;

					++num_empty_children;
				}
			}
		}

		// Clean up memory if all children are empty.
		if( num_empty_children == num_total_children ) {
			delete[] m_children;
			m_children = nullptr;
		}
	}

	// Continue cleanup at parent if requested.
	if( recursive && m_parent && !m_children && (!m_data || m_data->size() == 0) ) {
		m_parent->cleanup( true );
	}
}

}
