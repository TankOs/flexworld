#include <FlexWorld/Chunk.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestChunk ) {
	using namespace flex;

	Chunk::Vector size( 16, 16, 16 );

	// Check initial state.
	{
		Chunk chunk( size );
		BOOST_CHECK( chunk.get_size() == size );

		Chunk::Vector runner( 0, 0, 0 );
		bool all_empty( true );

		for( runner.z = 0; runner.z < size.z; ++runner.z ) {
			for( runner.y = 0; runner.y < size.y; ++runner.y ) {
				for( runner.x = 0; runner.x < size.x; ++runner.x ) {
					if( chunk.is_block_set( runner ) ) {
						all_empty = false;
					}
				}
			}
		}

		BOOST_CHECK( all_empty == true );
	}

	// Set blocks.
	{
		Chunk chunk( size );
		Chunk::Vector runner( 0, 0, 0 );
		bool all_sane( true );

		for( runner.z = 0; runner.z < size.z; ++runner.z ) {
			for( runner.y = 0; runner.y < size.y; ++runner.y ) {
				for( runner.x = 0; runner.x < size.x; ++runner.x ) {
					chunk.set_block( runner, runner.x );
				}
			}
		}

		for( runner.z = 0; runner.z < size.z; ++runner.z ) {
			for( runner.y = 0; runner.y < size.y; ++runner.y ) {
				for( runner.x = 0; runner.x < size.x; ++runner.x ) {
					if( chunk.get_block( runner ) != runner.x ) {
						all_sane = false;
					}
				}
			}
		}

		BOOST_CHECK( all_sane == true );
	}

	// Check clearing.
	{
		Chunk chunk( size );
		Chunk::Vector runner( 0, 0, 0 );
		bool all_sane( true );

		for( runner.z = 0; runner.z < size.z; ++runner.z ) {
			for( runner.y = 0; runner.y < size.y; ++runner.y ) {
				for( runner.x = 0; runner.x < size.x; ++runner.x ) {
					chunk.set_block( runner, runner.x );
				}
			}
		}

		chunk.clear();

		for( runner.z = 0; runner.z < size.z; ++runner.z ) {
			for( runner.y = 0; runner.y < size.y; ++runner.y ) {
				for( runner.x = 0; runner.x < size.x; ++runner.x ) {
					if( chunk.is_block_set( runner ) ) {
						all_sane = false;
					}
				}
			}
		}

		BOOST_CHECK( all_sane == true );
	}

	// Reset blocks.
	{
		Chunk chunk( size );

		chunk.set_block( Chunk::Vector( 5, 10, 15 ), 1337 );
		chunk.reset_block( Chunk::Vector( 5, 10, 15 ) );

		BOOST_CHECK( chunk.is_block_set( Chunk::Vector( 5, 10, 15 ) ) == false );
	}
}
