#include <FlexWorld/Chunk.hpp>

#include <boost/test/unit_test.hpp>

#include <iomanip> // XXX 

BOOST_AUTO_TEST_CASE( TestChunk ) {
	using namespace flex;

	static const Chunk::Vector SIZE( 16, 16, 16 );

	// Check initial state.
	{
		Chunk chunk( SIZE );
		BOOST_CHECK( chunk.get_size() == SIZE );

		Chunk::Vector runner( 0, 0, 0 );
		bool all_empty( true );

		for( runner.z = 0; runner.z < SIZE.z; ++runner.z ) {
			for( runner.y = 0; runner.y < SIZE.y; ++runner.y ) {
				for( runner.x = 0; runner.x < SIZE.x; ++runner.x ) {
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
		Chunk chunk( SIZE );
		Chunk::Vector runner( 0, 0, 0 );
		//Chunk::BlockFlags flags = 0;
		Chunk::Block id = 0;
		bool all_sane( true );

		for( runner.z = 0; runner.z < SIZE.z; ++runner.z ) {
			for( runner.y = 0; runner.y < SIZE.y; ++runner.y ) {
				for( runner.x = 0; runner.x < SIZE.x; ++runner.x ) {
					chunk.set_block( runner, id );

					id = static_cast<Chunk::Block>( (id + 1) % Chunk::MAX_BLOCK_ID );
					//flags = static_cast<Chunk::BlockFlags>( (flags + 1) % (1 << Chunk::FLAGS_BITS) );
				}
			}
		}

		// Check if everything has been set correctly. Also check raw data.
		id = 0;
		//flags = 0;
		std::size_t raw_idx = 0;

		for( runner.z = 0; runner.z < SIZE.z; ++runner.z ) {
			for( runner.y = 0; runner.y < SIZE.y; ++runner.y ) {
				for( runner.x = 0; runner.x < SIZE.x; ++runner.x ) {
					if( chunk.get_block( runner ) != id /*|| chunk.get_block_flags( runner ) != flags*/ ) {
						all_sane = false;
					}

					if(
						chunk.get_raw_data()[raw_idx] != id //||
						//(chunk.get_raw_data()[raw_idx] >> Chunk::ID_BITS) != flags
					) {
						all_sane = false;
					}

					id = static_cast<Chunk::Block>( (id + 1) % Chunk::MAX_BLOCK_ID );
					//flags = static_cast<Chunk::BlockFlags>( (flags + 1) % (1 << Chunk::FLAGS_BITS) );
					++raw_idx;
				}
			}
		}

		BOOST_CHECK( all_sane == true );
	}

	// Check clearing.
	{
		Chunk chunk( SIZE );
		Chunk::Vector runner( 0, 0, 0 );
		bool all_sane( true );

		for( runner.z = 0; runner.z < SIZE.z; ++runner.z ) {
			for( runner.y = 0; runner.y < SIZE.y; ++runner.y ) {
				for( runner.x = 0; runner.x < SIZE.x; ++runner.x ) {
					chunk.set_block( runner, 0 );
				}
			}
		}

		chunk.clear();

		for( runner.z = 0; runner.z < SIZE.z; ++runner.z ) {
			for( runner.y = 0; runner.y < SIZE.y; ++runner.y ) {
				for( runner.x = 0; runner.x < SIZE.x; ++runner.x ) {
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
		Chunk chunk( SIZE );

		chunk.set_block( Chunk::Vector( 5, 10, 15 ), 1337 );
		chunk.reset_block( Chunk::Vector( 5, 10, 15 ) );

		BOOST_CHECK( chunk.is_block_set( Chunk::Vector( 5, 10, 15 ) ) == false );
	}
}
