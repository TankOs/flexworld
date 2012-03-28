#include "Config.hpp"

#include <FlexWorld/PackageEnumerator.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

BOOST_AUTO_TEST_CASE( TestPackageEnumerator ) {
	using namespace flex;
	namespace fs = boost::filesystem;

	// Initial state.
	{
		PackageEnumerator enumerator;

		BOOST_CHECK( enumerator.get_num_script_files() == 0 );
	}

	// Enumerate non-existing directory.
	{
		PackageEnumerator enumerator;

		BOOST_CHECK( enumerator.enumerate( (fs::path( DATA_DIRECTORY ) / "does_not_exist").string() ) == false );
	}

	// Enumerate.
	{
		const fs::path base_dir = fs::path( DATA_DIRECTORY ) / "packages";

		PackageEnumerator enumerator;

		// Enumerate twice to make sure found files are cleared when enumerating
		// again.
		BOOST_CHECK( enumerator.enumerate( base_dir.string() ) );
		BOOST_CHECK( enumerator.enumerate( base_dir.string() ) );

		BOOST_CHECK( enumerator.get_num_script_files() == 2 );

		bool commands_script = false;
		bool another_script = false;

		for( std::size_t script_idx = 0; script_idx < enumerator.get_num_script_files(); ++script_idx ) {
			if( enumerator.get_script_file( script_idx ) == (base_dir / "test" / "commands.lua").string() ) {
				commands_script = true;
			}
			else if( enumerator.get_script_file( script_idx ) == (base_dir / "test" / "another.lua").string() ) {
				another_script = true;
			}
		}

		BOOST_CHECK( commands_script == true );
		BOOST_CHECK( another_script == true );
	}

}
