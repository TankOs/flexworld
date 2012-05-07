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

		BOOST_CHECK( &tree.insert( 1337, IntOctree::DataCuboid( 0, 0, 0, 32, 32, 32 ) ) == &tree );

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
			const IntOctree::DataCuboid& cuboid_,
			IntOctree::Quadrant quadrant_,
			int data_
		) :
			cuboid( cuboid_ ),
			quadrant( quadrant_ ),
			data( data_ )
		{
		}

		IntOctree::DataCuboid cuboid;
		IntOctree::Quadrant quadrant;
		int data;
	};

	// Add single data to each quadrant, forcing subdivision.
	{
		static const std::size_t TREE_SIZE = 32;
		static const IntOctree::DataCuboid::Type DATA_SIZE = TREE_SIZE / 2;

		std::vector<SingleDataQuadrantInfo> infos;

		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid(  0,  0,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_BOTTOM_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 16,  0,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_BOTTOM_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid(  0,  0, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_BOTTOM_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 16,  0, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_BOTTOM_NEAR, 4 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid(  0, 16,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_TOP_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 16, 16,  0, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_TOP_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid(  0, 16, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::LEFT_TOP_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 16, 16, 16, DATA_SIZE, DATA_SIZE, DATA_SIZE ), IntOctree::RIGHT_TOP_NEAR, 4 ) );

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
			BOOST_CHECK( child.get_data()[0] == info.data );

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
		static const IntOctree::DataCuboid::Type MAX_POS = TREE_SIZE - 1;

		std::vector<SingleDataQuadrantInfo> infos;

		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 0, 0, 0, 1, 1, 1 ), IntOctree::LEFT_BOTTOM_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( MAX_POS, 0, 0, 1, 1, 1 ), IntOctree::RIGHT_BOTTOM_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 0, 0, MAX_POS, 1, 1, 1 ), IntOctree::LEFT_BOTTOM_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( MAX_POS, 0, MAX_POS, 1, 1, 1 ), IntOctree::RIGHT_BOTTOM_NEAR, 4 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 0, MAX_POS, 0, 1, 1, 1 ), IntOctree::LEFT_TOP_FAR, 1 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( MAX_POS, MAX_POS, 0, 1, 1, 1 ), IntOctree::RIGHT_TOP_FAR, 2 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( 0, MAX_POS, MAX_POS, 1, 1, 1 ), IntOctree::LEFT_TOP_NEAR, 3 ) );
		infos.push_back( SingleDataQuadrantInfo( IntOctree::DataCuboid( MAX_POS, MAX_POS, MAX_POS, 1, 1, 1 ), IntOctree::RIGHT_TOP_NEAR, 4 ) );

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

			BOOST_CHECK( current->get_data()[0] == info.data );
		}
	}

	// Add multiple data to same node.
	{
		IntOctree tree( 4 );

		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, 4, 3, 2 ) );
		tree.insert( 2, IntOctree::DataCuboid( 1, 0, 0, 3, 1, 3 ) );
		tree.insert( 3, IntOctree::DataCuboid( 0, 0, 1, 1, 2, 3 ) );

		BOOST_CHECK( tree.get_num_data() == 3 );
		BOOST_CHECK( tree.is_subdivided() == false );

		IntOctree::DataArray data = tree.get_data();

		BOOST_REQUIRE( data.size() == 3 );
		BOOST_CHECK( data[0] == 1 );
		BOOST_CHECK( data[1] == 2 );
		BOOST_CHECK( data[2] == 3 );
	}

	// Search tree for multiple data in root node.
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 1, 2, 4, 3, 2 ) );
		tree.insert( 2, IntOctree::DataCuboid( 1, 0, 2, 3, 4, 2 ) );
		tree.insert( 3, IntOctree::DataCuboid( 2, 1, 0, 2, 3, 4 ) );

		IntOctree::DataArray results;

		tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );

		BOOST_REQUIRE( results.size() == 3 );
		BOOST_CHECK( results[0] == 1 );
		BOOST_CHECK( results[1] == 2 );
		BOOST_CHECK( results[2] == 3 );
	}

	// Search tree for data in child nodes (data aligned to node sizes).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );

		tree.insert( 1, IntOctree::DataCuboid(             0,             0,             0, 1, 1, 1 ) );
		tree.insert( 2, IntOctree::DataCuboid( TREE_SIZE - 1,             0,             0, 1, 1, 1 ) );
		tree.insert( 3, IntOctree::DataCuboid(             0,             0, TREE_SIZE - 1, 1, 1, 1 ) );
		tree.insert( 4, IntOctree::DataCuboid( TREE_SIZE - 1,             0, TREE_SIZE - 1, 1, 1, 1 ) );
		tree.insert( 5, IntOctree::DataCuboid(             0, TREE_SIZE - 1,             0, 1, 1, 1 ) );
		tree.insert( 6, IntOctree::DataCuboid( TREE_SIZE - 1, TREE_SIZE - 1,             0, 1, 1, 1 ) );
		tree.insert( 7, IntOctree::DataCuboid(             0, TREE_SIZE - 1, TREE_SIZE - 1, 1, 1, 1 ) );
		tree.insert( 8, IntOctree::DataCuboid( TREE_SIZE - 1, TREE_SIZE - 1, TREE_SIZE - 1, 1, 1, 1 ) );

		IntOctree::DataArray results;
		tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );

		BOOST_REQUIRE( results.size() == 8 );
		BOOST_CHECK( results[0] == 1 );
		BOOST_CHECK( results[1] == 2 );
		BOOST_CHECK( results[2] == 3 );
		BOOST_CHECK( results[3] == 4 );
		BOOST_CHECK( results[4] == 5 );
		BOOST_CHECK( results[5] == 6 );
		BOOST_CHECK( results[6] == 7 );
		BOOST_CHECK( results[7] == 8 );
	}

	// Search outside the tree.
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 2, 2, 2, 2, 2, 2 ) );

		IntOctree::DataArray results;

		tree.search( IntOctree::DataCuboid( 0, 0, 0, 2, 2, 2 ), results );
		BOOST_CHECK( results.size() == 0 );
	}

	// Remove data where data occupies whole octree (no subdivision).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 1 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 2, 2, 2, 1, 1, 1 ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 0 );
		}
	}

	// Remove data where data occupies whole octree (no subdivision) and multiple SAME data exists.
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 2 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 2, 2, 2, 1, 1, 1 ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 0 );
		}
	}

	// Remove data where data occupies whole octree (no subdivision) and multiple DIFFERENT data exists.
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );
		tree.insert( 2, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 2 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 2, 2, 2, 1, 1, 1 ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 1 );
		}
	}

	// Remove data where data is inserted at depth 1 (multiple data, same).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2 ) );
		tree.insert( 1, IntOctree::DataCuboid( TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2 ) );

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == false );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 2 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 0 );
		}

		BOOST_CHECK( tree.is_subdivided() == false );
	}

	// Remove data where data is inserted at depth 1 (multiple data, different).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2 ) );
		tree.insert( 2, IntOctree::DataCuboid( TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2, TREE_SIZE / 2 ) );

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == false );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 2 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 1 );
		}

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.has_child( IntOctree::LEFT_BOTTOM_FAR ) == false );
		BOOST_REQUIRE( tree.has_child( IntOctree::RIGHT_TOP_NEAR ) == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).is_subdivided() == false );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).get_num_data() == 1 );
	}

	// Remove data where data is inserted at depth 2 (multiple data, same).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2 ) );
		tree.insert( 1, IntOctree::DataCuboid( TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2 ) );

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == false );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 2 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 0 );
		}

		BOOST_CHECK( tree.is_subdivided() == false );
	}

	// Remove data where data is inserted at depth 2 (multiple data, different).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2 ) );
		tree.insert( 2, IntOctree::DataCuboid( TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2 ) );

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == false );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 2 );
		}

		tree.erase( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 1 );
		}

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.has_child( IntOctree::LEFT_BOTTOM_FAR ) == false );
		BOOST_REQUIRE( tree.has_child( IntOctree::RIGHT_TOP_NEAR ) == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).has_child( IntOctree::RIGHT_TOP_NEAR ) == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).get_child( IntOctree::RIGHT_TOP_NEAR ).is_subdivided() == false );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).get_child( IntOctree::RIGHT_TOP_NEAR ).get_num_data() == 1 );
	}

	// Remove data from specific node (single data, full tree coverage).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		IntOctree& node = tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ) );

		BOOST_REQUIRE( tree.is_subdivided() == false );

		node.erase( 1 );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 0 );
		}

		BOOST_CHECK( tree.is_subdivided() == false );
	}

	// Remove data from specific node (multi SAME data).
	{
		static const IntOctree::Size TREE_SIZE = 4;

		IntOctree tree( TREE_SIZE );
		IntOctree& node0 = tree.insert( 1, IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2 ) );
		IntOctree& node1 = tree.insert( 1, IntOctree::DataCuboid( TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE - TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2, TREE_SIZE / 2 / 2 ) );

		BOOST_REQUIRE( tree.is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::LEFT_BOTTOM_FAR ).get_child( IntOctree::LEFT_BOTTOM_FAR ).is_subdivided() == false );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).is_subdivided() == true );
		BOOST_REQUIRE( tree.get_child( IntOctree::RIGHT_TOP_NEAR ).get_child( IntOctree::RIGHT_TOP_NEAR ).is_subdivided() == false );

		node0.erase( 1 );

		// Verify.
		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 1 );
		}

		node1.erase( 1 );

		{
			IntOctree::DataArray results;

			tree.search( IntOctree::DataCuboid( 0, 0, 0, TREE_SIZE, TREE_SIZE, TREE_SIZE ), results );
			BOOST_REQUIRE( results.size() == 0 );
		}

		BOOST_CHECK( tree.is_subdivided() == false );
	}
}
