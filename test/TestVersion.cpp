#include <FlexWorld/Version.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestVersion ) {
	using namespace fw;

	// Initial.
	{
		Version version;

		BOOST_CHECK( version.get_major() == 0 );
		BOOST_CHECK( version.get_minor() == 0 );
		BOOST_CHECK( version.get_revision() == 0 );
	}

	// Ctor.
	{
		Version version( 1, 2, 3 );

		BOOST_CHECK( version.get_major() == 1 );
		BOOST_CHECK( version.get_minor() == 2 );
		BOOST_CHECK( version.get_revision() == 3 );
	}

	// Setter.
	{
		Version version;
		version.set_major( 1 );
		version.set_minor( 2 );
		version.set_revision( 3 );

		BOOST_CHECK( version.get_major() == 1 );
		BOOST_CHECK( version.get_minor() == 2 );
		BOOST_CHECK( version.get_revision() == 3 );
	}

	// Compare, greater, less.
	{
		Version version0( 0, 0, 0 );
		Version version1( 0, 0, 1 );
		Version version2( 0, 1, 0 );
		Version version3( 1, 0, 0 );
		Version version4( 0, 0, 0 );

		BOOST_CHECK( version0 == version4 );
		BOOST_CHECK( version0 != version1 );
		BOOST_CHECK( version1 > version0 );
		BOOST_CHECK( version0 < version1 );
		BOOST_CHECK( version2 > version1 );
		BOOST_CHECK( version1 < version2 );
		BOOST_CHECK( version3 > version2 );
		BOOST_CHECK( version2 < version3 );
	}
}
