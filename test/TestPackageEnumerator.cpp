#include "Config.hpp"

#include <FlexWorld/PackageEnumerator.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestPackageEnumerator ) {
	using namespace flex;

	// Initial state.
	{
		PackageEnumerator enumerator;

		BOOST_CHECK( enumerator.get_num_class_files() == 0 );
		BOOST_CHECK( enumerator.get_num_image_files() == 0 );
		BOOST_CHECK( enumerator.get_num_model_files() == 0 );
	}

	// Enumerate non-existing directory.
	{
		PackageEnumerator enumerator;

		BOOST_CHECK( enumerator.enumerate( DATA_DIRECTORY + "/does_not_exist/" ) == false );
	}

	// Enumerate.
	{
		const std::string base_dir = DATA_DIRECTORY + "/packages/";

		PackageEnumerator enumerator;
		BOOST_CHECK( enumerator.enumerate( base_dir ) );

		BOOST_CHECK( enumerator.get_num_class_files() == 2 );
		BOOST_CHECK( enumerator.get_num_image_files() == 2 );
		BOOST_CHECK( enumerator.get_num_model_files() == 1 );

		bool grass_image = false;
		bool grass_class = false;
		bool stone_image = false;
		bool stone_class = false;
		bool block_model = false;

		for( std::size_t class_idx = 0; class_idx < enumerator.get_num_class_files(); ++class_idx ) {
			if( enumerator.get_class_file( class_idx ) == base_dir + "test/grass.yml" ) {
				grass_class = true;
			}
			else if( enumerator.get_class_file( class_idx ) == base_dir + "test/stone.yml" ) {
				stone_class = true;
			}
		}

		for( std::size_t image_idx = 0; image_idx < enumerator.get_num_image_files(); ++image_idx ) {
			if( enumerator.get_image_file( image_idx ) == base_dir + "test/grass.png" ) {
				grass_image = true;
			}
			else if( enumerator.get_image_file( image_idx ) == base_dir + "test/stone.png" ) {
				stone_image = true;
			}
		}

		if( enumerator.get_model_file( 0 ) == base_dir + "test/block.fwm" ) {
			block_model = true;
		}

		BOOST_CHECK( grass_image == true );
		BOOST_CHECK( grass_class == true );
		BOOST_CHECK( stone_image == true );
		BOOST_CHECK( stone_class == true );
		BOOST_CHECK( block_model == true );
	}
}
