#include <FlexWorld/LooseOctree.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestLooseOctree ) {
	using namespace flex;

	typedef LooseOctree<int> IntOctree;

	// Initial state.
	{
		IntOctree tree( 128 );

		BOOST_CHECK( tree.get_size() == 128 );
		BOOST_CHECK( tree.get_position() == IntOctree::Vector( 0, 0, 0 ) );
		BOOST_CHECK( tree.is_subdivided() == false );
		BOOST_CHECK( tree.get_num_data() == 0 );

		BOOST_CHECK( tree.has_child( IntOctree::LEFT_BOTTOM_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_BOTTOM_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::LEFT_BOTTOM_NEAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_BOTTOM_NEAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::LEFT_TOP_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_TOP_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::LEFT_TOP_NEAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_TOP_NEAR ) == false );
	}

	// Add data that covers full root node.
	{
		IntOctree tree( 32 );

		BOOST_CHECK( &tree.insert( 1337, IntOctree::DataInfo::Cuboid( 0, 0, 0, 32, 32, 32 ) ) == &tree );

		BOOST_CHECK( tree.is_subdivided() == false );
		BOOST_CHECK( tree.get_num_data() == 1 );

		BOOST_CHECK( tree.has_child( IntOctree::LEFT_BOTTOM_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_BOTTOM_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::LEFT_BOTTOM_NEAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_BOTTOM_NEAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::LEFT_TOP_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_TOP_FAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::LEFT_TOP_NEAR ) == false );
		BOOST_CHECK( tree.has_child( IntOctree::RIGHT_TOP_NEAR ) == false );
	}

	struct SingleDataQuadrantInfo {
		SingleDataQuadrantInfo(
			const IntOctree::DataInfo::Cuboid& cuboid_,
			IntOctree::Quadrant quadrant_,
			int data_
		) :
			cuboid( cuboid_ ),
			quadrant( quadrant_ ),
			data( data_ )
		{
		}

		IntOctree::DataInfo::Cuboid cuboid;
		IntOctree::Quadrant quadrant;
		int data;
	};

	// Add single data to each quadrant, forcing subdivision.
	{
		static const std::size_t TREE_SIZE = 32;
		static const IntOctree::DataInfo::Scalar DATA_SIZE = TREE_SIZE / 2;

		std::vector<SingleDataQuadrantInfo> infos;

		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid(  0,  0,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_BOTTOM_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 16,  0,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_BOTTOM_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid(  0,  0, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_BOTTOM_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 16,  0, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_BOTTOM_NEAR, 4 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid(  0, 16,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_TOP_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 16, 16,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_TOP_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid(  0, 16, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_TOP_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 16, 16, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_TOP_NEAR, 4 ) );

		for( std::size_t idx = 0; idx < infos.size(); ++idx ) {
			IntOctree tree( TREE_SIZE );
			SingleDataQuadrantInfo& info = infos[idx];

			IntOctree::Vector tree_position(
				static_cast<IntOctree::Size>( info.cuboid.x ),
				static_cast<IntOctree::Size>( info.cuboid.y ),
				static_cast<IntOctree::Size>( info.cuboid.z )
			);

			BOOST_CHECK( &tree.insert( info.data, info.cuboid ) != &tree );

			BOOST_CHECK( tree.is_subdivided() == true );
			BOOST_CHECK( tree.get_num_data() == 0 );

			BOOST_REQUIRE( tree.has_child( info.quadrant ) == true );
			IntOctree& child = tree.get_child( info.quadrant );

			BOOST_CHECK( child.get_size() == TREE_SIZE / 2 );
			BOOST_CHECK( child.get_position() == tree_position );
			BOOST_CHECK( child.is_subdivided() == false );

			BOOST_REQUIRE( child.get_num_data() == 1 );
			BOOST_CHECK( child.get_data().front().data == info.data );
			BOOST_CHECK( child.get_data().front().cuboid == info.cuboid );

			BOOST_CHECK( tree.has_child( IntOctree::LEFT_BOTTOM_FAR ) == (info.quadrant == IntOctree::LEFT_BOTTOM_FAR) );
			BOOST_CHECK( tree.has_child( IntOctree::RIGHT_BOTTOM_FAR ) == (info.quadrant == IntOctree::RIGHT_BOTTOM_FAR) );
			BOOST_CHECK( tree.has_child( IntOctree::LEFT_BOTTOM_NEAR ) == (info.quadrant == IntOctree::LEFT_BOTTOM_NEAR) );
			BOOST_CHECK( tree.has_child( IntOctree::RIGHT_BOTTOM_NEAR ) == (info.quadrant == IntOctree::RIGHT_BOTTOM_NEAR) );
			BOOST_CHECK( tree.has_child( IntOctree::LEFT_TOP_FAR ) == (info.quadrant == IntOctree::LEFT_TOP_FAR) );
			BOOST_CHECK( tree.has_child( IntOctree::RIGHT_TOP_FAR ) == (info.quadrant == IntOctree::RIGHT_TOP_FAR) );
			BOOST_CHECK( tree.has_child( IntOctree::LEFT_TOP_NEAR ) == (info.quadrant == IntOctree::LEFT_TOP_NEAR) );
			BOOST_CHECK( tree.has_child( IntOctree::RIGHT_TOP_NEAR ) == (info.quadrant == IntOctree::RIGHT_TOP_NEAR) );
		}
	}

	// Add single data with smallest possible size to each quadrant, forcing
	// maximum number of subdivisions.
	{
		static const IntOctree::Size TREE_SIZE = 8;
		static const IntOctree::DataInfo::Scalar MAX_POS = TREE_SIZE - 1;

		std::vector<SingleDataQuadrantInfo> infos;

		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 0, 0, 0, 1, 1, 1 ), IntOctree::LEFT_BOTTOM_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( MAX_POS, 0, 0, 1, 1, 1 ), IntOctree::RIGHT_BOTTOM_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 0, 0, MAX_POS, 1, 1, 1 ), IntOctree::LEFT_BOTTOM_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( MAX_POS, 0, MAX_POS, 1, 1, 1 ), IntOctree::RIGHT_BOTTOM_NEAR, 4 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 0, MAX_POS, 0, 1, 1, 1 ), IntOctree::LEFT_TOP_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( MAX_POS, MAX_POS, 0, 1, 1, 1 ), IntOctree::RIGHT_TOP_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( 0, MAX_POS, MAX_POS, 1, 1, 1 ), IntOctree::LEFT_TOP_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataInfo::Cuboid( MAX_POS, MAX_POS, MAX_POS, 1, 1, 1 ), IntOctree::RIGHT_TOP_NEAR, 4 ) );

		for( std::size_t idx = 0; idx < infos.size(); ++idx ) {
			SingleDataQuadrantInfo& info = infos[idx];
			IntOctree tree( TREE_SIZE );

			BOOST_CHECK( &tree.insert( info.data, info.cuboid ) != &tree );

			BOOST_CHECK( tree.is_subdivided() == true );
			BOOST_CHECK( tree.get_num_data() == 0 );

			BOOST_REQUIRE( tree.has_child( info.quadrant ) == true );

			// Traverse to deepest child.
			IntOctree* current = &tree;

			while( current->has_child( info.quadrant ) ) {
				current = &current->get_child( info.quadrant );

				if( current->get_size() > 1 ) {
					BOOST_CHECK( current->get_num_data() == 0 );
					BOOST_CHECK( current->is_subdivided() == true );
				}
			}

			IntOctree::Vector tree_position(
				static_cast<IntOctree::Size>( info.cuboid.x ),
				static_cast<IntOctree::Size>( info.cuboid.y ),
				static_cast<IntOctree::Size>( info.cuboid.z )
			);

			BOOST_REQUIRE( current->get_size() == 1 );
			BOOST_CHECK( current->get_position() == tree_position );
			BOOST_CHECK( current->is_subdivided() == false );

			BOOST_CHECK( current->get_data().front().data == info.data );
			BOOST_CHECK( current->get_data().front().cuboid == info.cuboid );
		}
	}

	// Add multiple data to same node.
	{
		IntOctree tree( 4 );

		tree.insert( 1, IntOctree::DataInfo::Cuboid( 0, 0, 0, 4, 3, 2 ) );
		tree.insert( 2, IntOctree::DataInfo::Cuboid( 1, 0, 0, 3, 1, 3 ) );
		tree.insert( 3, IntOctree::DataInfo::Cuboid( 0, 0, 1, 1, 2, 3 ) );

		BOOST_CHECK( tree.get_num_data() == 3 );
		BOOST_CHECK( tree.is_subdivided() == false );

		IntOctree::DataList::const_iterator data_iter = tree.get_data().begin();

		BOOST_CHECK( data_iter->data == 1 );
		BOOST_CHECK( data_iter->cuboid == IntOctree::DataInfo::Cuboid( 0, 0, 0, 4, 3, 2 ) );
		++data_iter;

		BOOST_CHECK( data_iter->data == 2 );
		BOOST_CHECK( data_iter->cuboid == IntOctree::DataInfo::Cuboid( 1, 0, 0, 3, 1, 3 ) );
		++data_iter;

		BOOST_CHECK( data_iter->data == 3 );
		BOOST_CHECK( data_iter->cuboid == IntOctree::DataInfo::Cuboid( 0, 0, 1, 1, 2, 3 ) );
		++data_iter;
	}

	// Search tree for single data in root node.
	{
		static const IntOctree::DataInfo::Cuboid CUBOID( 0, 1, 2, 4, 3, 2 );
		static const int DATA = 1337;
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );

		tree.insert( DATA, CUBOID );

		IntOctree::ResultArray results;

		tree.search( IntOctree::DataInfo::Cuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );

		//BOOST_CHECK( results.size() == 1 );
	}
}
