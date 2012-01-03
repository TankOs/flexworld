#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/Entity.hpp>

#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_CASE( TestPlanet ) {
	using namespace flex;

	const Planet::Vector PLANET_SIZE( 2, 1, 2 );
	const Chunk::Vector CHUNK_SIZE( 16, 16, 16 );

	// Create planet and check initial state.
	{
		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );

		BOOST_CHECK( planet.get_id() == "construct" );
		BOOST_CHECK( planet.get_size() == PLANET_SIZE );
		BOOST_CHECK( planet.get_chunk_size() == CHUNK_SIZE );
		BOOST_CHECK( planet.get_num_entities() == 0 );
	}

	// Transform coordinate.
	{
		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );

		// Check all possible coordinates.
		sf::Vector3f coordinate( 0, 0, 0 );
		Planet::Vector chunk_position( 0, 0, 0 );
		Chunk::Vector block_position( 0, 0, 0 );

		const sf::Vector3f FLOAT_CHUNK_SIZE( CHUNK_SIZE.x, CHUNK_SIZE.y, CHUNK_SIZE.z );
		Planet::Vector real_chunk_position( 0, 0, 0 );
		Chunk::Vector real_block_position( 0, 0, 0 );

		for( coordinate.z = 0; coordinate.z < static_cast<float>( PLANET_SIZE.z * CHUNK_SIZE.z ); coordinate.z += 0.5f ) {
			real_chunk_position.z = static_cast<Planet::ScalarType>( coordinate.z / FLOAT_CHUNK_SIZE.z );
			real_block_position.z = static_cast<Chunk::ScalarType>( static_cast<unsigned int>( coordinate.z ) % CHUNK_SIZE.z );

			for( coordinate.y = 0; coordinate.y < static_cast<float>( PLANET_SIZE.y * CHUNK_SIZE.y ); coordinate.y += 0.5f ) {
				real_chunk_position.y = static_cast<Planet::ScalarType>( coordinate.y / FLOAT_CHUNK_SIZE.y );
				real_block_position.y = static_cast<Chunk::ScalarType>( static_cast<unsigned int>( coordinate.y ) % CHUNK_SIZE.y );

				for( coordinate.x = 0; coordinate.x < static_cast<float>( PLANET_SIZE.x * CHUNK_SIZE.x ); coordinate.x += 0.5f ) {
					real_chunk_position.x = static_cast<Planet::ScalarType>( coordinate.x / FLOAT_CHUNK_SIZE.x );
					real_block_position.x = static_cast<Chunk::ScalarType>( static_cast<unsigned int>( coordinate.x ) % CHUNK_SIZE.x );

					BOOST_CHECK( planet.transform( coordinate, chunk_position, block_position ) );
					BOOST_CHECK( chunk_position == real_chunk_position );
					BOOST_CHECK( block_position == real_block_position );
				}
			}
		}

		// Check invalid coordinate.
		sf::Vector3f invalid_coordinate(
			static_cast<float>( PLANET_SIZE.x * CHUNK_SIZE.x ),
			static_cast<float>( PLANET_SIZE.y * CHUNK_SIZE.y ) - 1.0f,
			static_cast<float>( PLANET_SIZE.z * CHUNK_SIZE.z ) - 1.0f
		);

		BOOST_CHECK( planet.transform( invalid_coordinate, chunk_position, block_position ) == false );
	}

	// Check for non-existing chunks.
	{
		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );

		for( Planet::ScalarType z = 0; z < PLANET_SIZE.z; ++z ) {
			for( Planet::ScalarType y = 0; y < PLANET_SIZE.y; ++y ) {
				for( Planet::ScalarType x = 0; x < PLANET_SIZE.x; ++x ) {
					BOOST_CHECK( planet.has_chunk( Planet::Vector( x, y, z ) ) == false );
				}
			}
		}
	}

	// Create chunk.
	{
		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );
		const Planet::Vector POSITION( 0, 0, 0 );

		planet.create_chunk( POSITION );
		BOOST_CHECK( planet.get_num_chunks() == 1 );
		BOOST_CHECK( planet.has_chunk( POSITION ) == true );
	}

	// Set some blocks.
	{
		// Prepare classes.
		const std::size_t NUM_CLASSES = 5;

		FlexID ids[NUM_CLASSES];
		std::vector<Class> classes;

		for( std::size_t index = 0; index < NUM_CLASSES; ++index ) {
			std::stringstream sstr;
			sstr << "fw.base/grass" << index << ".yml";

			BOOST_REQUIRE( ids[index].parse( sstr.str() ) == true );
			classes.push_back( Class( ids[index] ) );
		}

		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );
		Planet::Coordinate coord( 0, 0, 0 );
		Planet::Vector chunk_pos( 0, 0, 0 );
		Chunk::Vector block_pos( 0, 0, 0 );
		std::size_t class_index( 0 );

		for( chunk_pos.z = 0; chunk_pos.z < PLANET_SIZE.z; ++chunk_pos.z ) {
			for( chunk_pos.y = 0; chunk_pos.y < PLANET_SIZE.y; ++chunk_pos.y ) {
				for( chunk_pos.x = 0; chunk_pos.x < PLANET_SIZE.x; ++chunk_pos.x ) {
					// Create chunk.
					planet.create_chunk( chunk_pos );

					for( block_pos.z = 0; block_pos.z < CHUNK_SIZE.z; ++block_pos.z ) {
						for( block_pos.y = 0; block_pos.y < CHUNK_SIZE.y; ++block_pos.y ) {
							for( block_pos.x = 0; block_pos.x < CHUNK_SIZE.x; ++block_pos.x ) {
								// Set block.
								planet.set_block( chunk_pos, block_pos, classes[class_index] );

								// Select next class.
								++class_index;
								if( class_index >= NUM_CLASSES ) {
									class_index = 0;
								}
							}
						}
					}
				}
			}
		}

		// Verify set blocks.
		class_index = 0;

		for( chunk_pos.z = 0; chunk_pos.z < PLANET_SIZE.z; ++chunk_pos.z ) {
			for( chunk_pos.y = 0; chunk_pos.y < PLANET_SIZE.y; ++chunk_pos.y ) {
				for( chunk_pos.x = 0; chunk_pos.x < PLANET_SIZE.x; ++chunk_pos.x ) {
					for( block_pos.z = 0; block_pos.z < CHUNK_SIZE.z; ++block_pos.z ) {
						for( block_pos.y = 0; block_pos.y < CHUNK_SIZE.y; ++block_pos.y ) {
							for( block_pos.x = 0; block_pos.x < CHUNK_SIZE.x; ++block_pos.x ) {
								// Verify block.
								BOOST_CHECK( planet.find_block( chunk_pos, block_pos ) == &classes[class_index] );

								// Select next class.
								++class_index;
								if( class_index >= NUM_CLASSES ) {
									class_index = 0;
								}
							}
						}
					}
				}
			}
		}

	}

	// Check resetting blocks.
	{
		FlexID id;
		id.parse( "fw.base/grass.yml" );

		Class cls( id );

		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );
		const Planet::Vector CHUNK_POSITION( 0, 0, 0 );
		const Chunk::Vector BLOCK_POSITION( 0, 0, 0 );

		planet.create_chunk( CHUNK_POSITION );
		planet.set_block( CHUNK_POSITION, BLOCK_POSITION, cls );

		BOOST_CHECK( planet.find_block( CHUNK_POSITION, BLOCK_POSITION ) == &cls );

		planet.reset_block( CHUNK_POSITION, BLOCK_POSITION );
		BOOST_CHECK( planet.find_block( CHUNK_POSITION, BLOCK_POSITION ) == nullptr );
	}

	// Entities.
	{
		FlexID id;
		id.parse( "fw.weapons/sword" );

		Class cls( id );
		Entity ent( cls );

		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );

		BOOST_CHECK( planet.has_entity( ent ) == false );

		planet.add_entity( ent );

		BOOST_CHECK( planet.get_num_entities() == 1 );
		BOOST_CHECK( planet.has_entity( ent ) == true );

		// Add another entity.
		Entity ent2( cls );
		ent2.set_id( 2 );
		planet.add_entity( ent2 );

		BOOST_CHECK( planet.get_num_entities() == 2 );
		BOOST_CHECK( planet.has_entity( ent ) == true );
		BOOST_CHECK( planet.has_entity( ent2 ) == true );

		// Remove entity.
		planet.remove_entity( ent );
		BOOST_CHECK( planet.get_num_entities() == 1 );
		BOOST_CHECK( planet.has_entity( ent ) == false );
		BOOST_CHECK( planet.has_entity( ent2 ) == true );

		planet.remove_entity( ent2 );
		BOOST_CHECK( planet.get_num_entities() == 0 );
		BOOST_CHECK( planet.has_entity( ent2 ) == false );
	}

	// Clear planet.
	{
		Planet planet( "construct", PLANET_SIZE, CHUNK_SIZE );
		const Planet::Vector POSITION( 0, 0, 0 );

		FlexID id;
		id.parse( "fw.weapons/sword" );
		Class cls( id );
		Entity ent( cls );
		
		planet.create_chunk( POSITION );
		planet.add_entity( ent );

		planet.clear();

		// Make sure properties are still valid.
		BOOST_CHECK( planet.get_id() == "construct" );
		BOOST_CHECK( planet.get_size() == PLANET_SIZE );
		BOOST_CHECK( planet.get_chunk_size() == CHUNK_SIZE );

		// Check previously created chunk is destroyed.
		BOOST_CHECK( planet.get_num_chunks() == 0 );
		BOOST_CHECK( planet.has_chunk( POSITION ) == false );

		// Check all entities are gone.
		BOOST_CHECK( planet.get_num_entities() == 0 );
	}
}
