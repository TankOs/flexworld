#include <FlexWorld/Cuboid.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestCuboid ) {
	using namespace flex;

	// Initial state.
	{
		FloatCuboid cuboid;

		BOOST_CHECK( cuboid.x == 0.0f );
		BOOST_CHECK( cuboid.y == 0.0f );
		BOOST_CHECK( cuboid.z == 0.0f );
		BOOST_CHECK( cuboid.width == 0.0f );
		BOOST_CHECK( cuboid.height == 0.0f );
		BOOST_CHECK( cuboid.depth == 0.0f );
	}

	// Ctor.
	{
		FloatCuboid cuboid( 1, 2, 3, 4, 5, 6 );

		BOOST_CHECK( cuboid.x == 1.0f );
		BOOST_CHECK( cuboid.y == 2.0f );
		BOOST_CHECK( cuboid.z == 3.0f );
		BOOST_CHECK( cuboid.width == 4.0f );
		BOOST_CHECK( cuboid.height == 5.0f );
		BOOST_CHECK( cuboid.depth == 6.0f );
	}

	// Equality.
	{
		FloatCuboid c0( 1, 2, 3, 4, 5, 6 );
		FloatCuboid c1( 1, 2, 3, 4, 5, 6 );

		BOOST_CHECK( c0 == c1 );
	}

	// Unequality.
	{
		FloatCuboid c0( 1, 2, 3, 4, 5, 6 );

		FloatCuboid c1( 0, 2, 3, 4, 5, 6 );
		BOOST_CHECK( c0 != c1 );

		c1 = c0;
		c1.y = 0;
		BOOST_CHECK( c0 != c1 );

		c1 = c0;
		c1.z = 0;
		BOOST_CHECK( c0 != c1 );

		c1 = c0;
		c1.width = 0;
		BOOST_CHECK( c0 != c1 );

		c1 = c0;
		c1.height = 0;
		BOOST_CHECK( c0 != c1 );

		c1 = c0;
		c1.depth = 0;
		BOOST_CHECK( c0 != c1 );
	}
}
