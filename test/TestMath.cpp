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

	// Degrees to radians.
	{
		BOOST_CHECK( deg_to_rad( 0 ) == 0 );
		BOOST_CHECK( std::abs( deg_to_rad( 90 ) - 1.57079633f ) < 0.0000001f);
		BOOST_CHECK( std::abs( deg_to_rad( 180 ) - 3.14159265f ) < 0.0000001f);
		BOOST_CHECK( std::abs( deg_to_rad( 270 ) - 4.71238898f ) < 0.0000001f);
		BOOST_CHECK( std::abs( deg_to_rad( 360 ) - 6.28318531f ) < 0.0000001f);
	}

	// Convert polar coordinate to vector.
	{
		static const float FAULT_TOLERANCE = 0.0001f;

		sf::Vector3f result;

		result = polar_to_vector( deg_to_rad( 0.0f ), deg_to_rad( 0.0f ), 1.0f );
		BOOST_CHECK( std::abs( 0.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 1.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.z ) <= FAULT_TOLERANCE );

		result = polar_to_vector( deg_to_rad( 90.0f ), deg_to_rad( 0.0f ), 1.0f );
		BOOST_CHECK( std::abs( 0.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( -1.0f - result.z ) <= FAULT_TOLERANCE );

		result = polar_to_vector( deg_to_rad( 180.0f ), deg_to_rad( 0.0f ), 1.0f );
		BOOST_CHECK( std::abs( 0.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( -1.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.z ) <= FAULT_TOLERANCE );

		result = polar_to_vector( deg_to_rad( 270.0f ), deg_to_rad( 0.0f ), 1.0f );
		BOOST_CHECK( std::abs( 0.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 1.0f - result.z ) <= FAULT_TOLERANCE );

		result = polar_to_vector( deg_to_rad( 90.0f ), deg_to_rad( 90.0f ), 1.0f );
		BOOST_CHECK( std::abs( -1.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.z ) <= FAULT_TOLERANCE );

		result = polar_to_vector( deg_to_rad( 90.0f ), deg_to_rad( 180.0f ), 1.0f );
		BOOST_CHECK( std::abs( 0.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 1.0f - result.z ) <= FAULT_TOLERANCE );

		result = polar_to_vector( deg_to_rad( 90.0f ), deg_to_rad( 270.0f ), 1.0f );
		BOOST_CHECK( std::abs( 1.0f - result.x ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.y ) <= FAULT_TOLERANCE );
		BOOST_CHECK( std::abs( 0.0f - result.z ) <= FAULT_TOLERANCE );
	}

	// Dot product.
	{
		sf::Vector3f result = dot_product(
			sf::Vector3f( 1, 2, 3 ),
			sf::Vector3f( 3, 2, 1 )
		);

		BOOST_CHECK(
			result.x == -4 &&
			result.y == 8 &&
			result.z == -4
		);
	}

	// Signum.
	{
		BOOST_CHECK( signum( -10 ) == -1 );
		BOOST_CHECK( signum( 0 ) == 0 );
		BOOST_CHECK( signum( 10 ) == 1 );
	}
}
