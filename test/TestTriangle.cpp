#include <FlexWorld/Triangle.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestTriangle ) {
	using namespace flex;

	// Initial state.
	{
		Triangle tri;

		BOOST_CHECK( tri.vertices[0] == 0 );
		BOOST_CHECK( tri.vertices[1] == 0 );
		BOOST_CHECK( tri.vertices[2] == 0 );
	}

	// Equal & unequal.
	{
		Triangle t0( 0, 1, 2 );
		Triangle t1( 0, 1, 2 );

		BOOST_CHECK( t0 == t1 );

		t1 = Triangle( 9, 1, 2 );
		BOOST_CHECK( t0 != t1 );

		t1 = Triangle( 0, 9, 2 );
		BOOST_CHECK( t0 != t1 );

		t1 = Triangle( 0, 1, 9 );
		BOOST_CHECK( t0 != t1 );
	}
}
