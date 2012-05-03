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

	// Intersections.
	{
		// Inner cuboid fully covered by outer cuboid.
		{
			FloatCuboid one( 0, 0, 0, 10, 10, 10 );
			FloatCuboid two( 1, 2, 3, 4, 5, 6 );

			FloatCuboid intersection = FloatCuboid::calc_intersection( one, two );

			BOOST_CHECK( intersection.x == 1 );
			BOOST_CHECK( intersection.y == 2 );
			BOOST_CHECK( intersection.z == 3 );
			BOOST_CHECK( intersection.width == 4 );
			BOOST_CHECK( intersection.height == 5 );
			BOOST_CHECK( intersection.depth == 6 );
		}

		// Intersecting cuboid is larger on x axis, smaller on remaining axes.
		{
			FloatCuboid one( 0, 0, 0, 10, 10, 10 );
			FloatCuboid two( -5, 1, 1, 20, 8, 8 );

			FloatCuboid intersection = FloatCuboid::calc_intersection( one, two );

			BOOST_CHECK( intersection.x == 0 );
			BOOST_CHECK( intersection.y == 1 );
			BOOST_CHECK( intersection.z == 1 );
			BOOST_CHECK( intersection.width == 10 );
			BOOST_CHECK( intersection.height == 8 );
			BOOST_CHECK( intersection.depth == 8 );
		}

		// Intersecting cuboid is larger on y axis, smaller on remaining axes.
		{
			FloatCuboid one( 0, 0, 0, 10, 10, 10 );
			FloatCuboid two( 1, -5, 1, 8, 20, 8 );

			FloatCuboid intersection = FloatCuboid::calc_intersection( one, two );

			BOOST_CHECK( intersection.x == 1 );
			BOOST_CHECK( intersection.y == 0 );
			BOOST_CHECK( intersection.z == 1 );
			BOOST_CHECK( intersection.width == 8 );
			BOOST_CHECK( intersection.height == 10 );
			BOOST_CHECK( intersection.depth == 8 );
		}

		// Intersecting cuboid is larger on z axis, smaller on remaining axes.
		{
			FloatCuboid one( 0, 0, 0, 10, 10, 10 );
			FloatCuboid two( 1, 1, -5, 8, 8, 20 );

			FloatCuboid intersection = FloatCuboid::calc_intersection( one, two );

			BOOST_CHECK( intersection.x == 1 );
			BOOST_CHECK( intersection.y == 1 );
			BOOST_CHECK( intersection.z == 0 );
			BOOST_CHECK( intersection.width == 8 );
			BOOST_CHECK( intersection.height == 8 );
			BOOST_CHECK( intersection.depth == 10 );
		}

		// Same cuboids.
		{
			FloatCuboid one( 0, 0, 0, 3, 3, 3 );
			FloatCuboid two( 0, 0, 0, 3, 3, 3 );

			FloatCuboid intersection = FloatCuboid::calc_intersection( one, two );

			BOOST_CHECK( intersection == FloatCuboid( 0, 0, 0, 3, 3, 3 ) );
		}

		// No intersection.
		{
			FloatCuboid one( 0, 0, 0, 1, 1, 1 );
			FloatCuboid two( 1, 1, 1, 1, 1, 1 );

			FloatCuboid intersection = FloatCuboid::calc_intersection( one, two );

			BOOST_CHECK( intersection == FloatCuboid( 0, 0, 0, 0, 0, 0 ) );
		}
	}
}
