#include <FlexWorld/Math.hpp>

#include <SFML/System/Vector2.hpp>
#include <boost/test/unit_test.hpp>
#include <cmath>

BOOST_AUTO_TEST_CASE( TestMath ) {
	using namespace flex;

	// Normalize vector.
	{
		sf::Vector3f vec( 23.f, 94.f, 38.f );

		float length = std::sqrt( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
		sf::Vector3f expected( vec.x / length, vec.y / length, vec.z / length );

		flex::normalize( vec );

		BOOST_CHECK( vec == expected );
	}

	// Calc area of 2D triangle.
	{
		sf::Vector2f points[3] = {
			sf::Vector2f( 5, 5 ),
			sf::Vector2f( 10, 5 ),
			sf::Vector2f( 10, 10 )
		};

		BOOST_CHECK( flex::calc_triangle_area( points[0], points[1], points[2] ) == 12.5f );
	}

	// Calc area of 2D rect.
	{
		float a = 10.0f;
		float b = 30.0f;

		BOOST_CHECK( flex::calc_rect_area( a, b ) == a * b );
	}
}
