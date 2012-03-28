#include "Config.hpp"

#include <FlexWorld/GameModeDriver.hpp>
#include <FlexWorld/GameMode.hpp>

#include <boost/test/unit_test.hpp>
#include <fstream>

BOOST_AUTO_TEST_CASE( TestGameModeDriver ) {
	using namespace flex;

	// Read test game mode.
	std::ifstream in( (DATA_DIRECTORY + "/modes/tutorial.yml").c_str() );
	BOOST_REQUIRE( in.is_open() == true );

	std::stringstream buffer;
	buffer << in.rdbuf();
	BOOST_REQUIRE( in );

	// Load game mode.
	{
		GameMode mode = GameModeDriver::deserialize( buffer.str() );

		BOOST_CHECK( mode.get_name() == "Tutorial" );
		BOOST_CHECK( mode.get_author() == "FlexWorld Team" );
		BOOST_CHECK( mode.get_website() == "http://flexworld-game.com/" );
		BOOST_CHECK( mode.get_version() == Version( 1, 2, 3 ) );
		BOOST_CHECK( mode.get_min_flexworld_version() == Version( 1, 0, 0 ) );
		BOOST_CHECK( mode.get_max_flexworld_version() == Version( 1, 2, 1 ) );
		BOOST_CHECK( mode.get_thumbnail() == FlexID::make( "fw.gm.tutorial/thumb.png" ) );
		BOOST_CHECK( mode.get_default_entity_class_id() == FlexID::make( "fw.base.human/dwarf_male" ) );

		BOOST_REQUIRE( mode.get_num_packages() == 3 );

		BOOST_CHECK( mode.get_package( 0 ) == FlexID::make( "fw.lib.admin" ) );
		BOOST_CHECK( mode.get_package( 1 ) == FlexID::make( "fw.base" ) );
		BOOST_CHECK( mode.get_package( 2 ) == FlexID::make( "fw.gm.tutorial" ) );
	}
}
