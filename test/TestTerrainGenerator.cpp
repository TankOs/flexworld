#include <FlexWorld/TerrainGenerator.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Class.hpp>

#include <SFML/Graphics/Image.hpp> // XXX 
#include <boost/test/unit_test.hpp>
#include <ctime>

BOOST_AUTO_TEST_CASE( TestTerrainGenerator ) {
	using namespace flex;

	static const Class default_cls( FlexID::make( "fw.base.nature/grass" ) );
	static const Class cls0( FlexID::make( "fw.base.nature/cls0" ) );
	static const Class cls1( FlexID::make( "fw.base.nature/cls1" ) );

	// Initial state.
	{
		TerrainGenerator gen( default_cls );

		BOOST_CHECK( &gen.get_default_class() == &default_cls );
		BOOST_CHECK( gen.get_num_layers() == 0 );
		BOOST_CHECK( gen.get_seed() == 0 );
		BOOST_CHECK( gen.get_base_height() == 0 );
		BOOST_CHECK( gen.get_maximum_height() == 0 );

		TerrainGenerator::Layer layer;

		BOOST_CHECK( layer.min_height == 0 );
		BOOST_CHECK( layer.max_height == 0 );
		BOOST_CHECK( layer.cls == nullptr );
	}

	// Basic properties.
	{
		TerrainGenerator gen( default_cls );

		gen.set_seed( 12345 );
		gen.set_base_height( 4455 );
		gen.set_maximum_height( 9524 );

		BOOST_CHECK( gen.get_seed() == 12345 );
		BOOST_CHECK( gen.get_base_height() == 4455 );
		BOOST_CHECK( gen.get_maximum_height() == 9524 );
	}

	// Check layer equality.
	{
		TerrainGenerator::Layer layer0;
		TerrainGenerator::Layer layer1;

		BOOST_CHECK( layer0 == layer1 );

		layer1.min_height = 10;
		BOOST_CHECK( (layer0 == layer1) == false );

		layer0 = layer1;
		layer1.max_height = 10;
		BOOST_CHECK( (layer0 == layer1) == false );

		layer0 = layer1;
		layer1.cls = &cls1;
		BOOST_CHECK( (layer0 == layer1) == false );

		layer0 = layer1;
		BOOST_CHECK( layer0 == layer1 );
	}

	// Add layers.
	{
		TerrainGenerator gen( default_cls );

		TerrainGenerator::Layer layer0;

		layer0.min_height = 0;
		layer0.max_height = 10;
		layer0.cls = &cls0;
		gen.add_layer( layer0 );

		BOOST_CHECK( gen.get_num_layers() == 1 );
		BOOST_CHECK( gen.get_layer( 0 ) == layer0 );

		TerrainGenerator::Layer layer1;

		layer1.min_height = 10;
		layer1.max_height = 20;
		layer1.cls = &cls1;
		gen.add_layer( layer1 );

		BOOST_CHECK( gen.get_num_layers() == 2 );
		BOOST_CHECK( gen.get_layer( 0 ) == layer0 );
		BOOST_CHECK( gen.get_layer( 1 ) == layer1 );
	}

	// Generate terrain.
	// Some notes on testing the generation process: A whole terrain is generated
	// 5 times with random seeds. Therefore it's not possible to test the actual
	// output. Only special output properties like max. height, layers etc. are
	// tested.
	{
		std::srand( static_cast<unsigned int>( std::time( 0 ) ) );

		// Generate a terrain with default ID only.
		{
			Planet planet( "gen", Planet::Vector( 4, 4, 4 ), Chunk::Vector( 16, 16, 16 ) );
			TerrainGenerator gen( default_cls );

			gen.set_seed( std::rand() );
			gen.set_base_height( 0 );
			gen.set_maximum_height( 40 );

			gen.generate( planet, Cuboid<uint32_t>( 0, 0, 0, 64, 64, 64 ) );
		}
	}
}
