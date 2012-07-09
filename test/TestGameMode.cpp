#include <FlexWorld/GameMode.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestGameMode ) {
	using namespace fw;

	// Initial state.
	{
		GameMode mode;

		BOOST_CHECK( mode.get_name() == "" );
		BOOST_CHECK( mode.get_author() == "" );
		BOOST_CHECK( mode.get_website() == "" );
		BOOST_CHECK( mode.get_version() == Version( 0, 0, 0 ) );
		BOOST_CHECK( mode.get_min_flexworld_version() == Version( 0, 0, 0 ) );
		BOOST_CHECK( mode.get_max_flexworld_version() == Version( 0, 0, 0 ) );
		BOOST_CHECK( mode.get_thumbnail().is_valid_package() == false );
		BOOST_CHECK( mode.get_default_entity_class_id().is_valid_package() == false );
		BOOST_CHECK( mode.get_num_packages() == 0 );
	}

	// Basic properties.
	{
		GameMode mode;

		mode.set_name( "Tutorial" );
		mode.set_author( "FlexWorld Team" );
		mode.set_website( "http://flexworld-game.com/" );
		mode.set_version( Version( 1, 2, 3 ) );
		mode.set_min_flexworld_version( Version( 1, 0, 0 ) );
		mode.set_max_flexworld_version( Version( 1, 2, 1 ) );
		mode.set_thumbnail( FlexID::make( "fw.gm.tutorial/thumb.png" ) );
		mode.set_default_entity_class_id( FlexID::make( "fw.base.human/dwarf_male" ) );

		BOOST_CHECK( mode.get_name() == "Tutorial" );
		BOOST_CHECK( mode.get_author() == "FlexWorld Team" );
		BOOST_CHECK( mode.get_website() == "http://flexworld-game.com/" );
		BOOST_CHECK( mode.get_version() == Version( 1, 2, 3 ) );
		BOOST_CHECK( mode.get_min_flexworld_version() == Version( 1, 0, 0 ) );
		BOOST_CHECK( mode.get_max_flexworld_version() == Version( 1, 2, 1 ) );
		BOOST_CHECK( mode.get_thumbnail() == FlexID::make( "fw.gm.tutorial/thumb.png" ) );
		BOOST_CHECK( mode.get_default_entity_class_id() == FlexID::make( "fw.base.human/dwarf_male" ) );
		BOOST_CHECK( mode.get_num_packages() == 0 );
	}

	// Packages.
	{
		GameMode mode;

		BOOST_CHECK( mode.get_num_packages() == 0 );

		mode.add_package( FlexID::make( "fw.basic" ) );
		mode.add_package( FlexID::make( "fw.gm.tutorial" ) );
		mode.add_package( FlexID::make( "fw.lib.admin" ) );

		BOOST_CHECK( mode.get_num_packages() == 3 );

		BOOST_CHECK( mode.get_package( 0 ).get() == "fw.basic" );
		BOOST_CHECK( mode.get_package( 1 ).get() == "fw.gm.tutorial" );
		BOOST_CHECK( mode.get_package( 2 ).get() == "fw.lib.admin" );
	}
}
