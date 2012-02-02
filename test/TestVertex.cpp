#include <FlexWorld/Vertex.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestVertex ) {
	using namespace flex;

	// Initial state.
	{
		Vertex vertex;

		BOOST_CHECK( vertex.position == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( vertex.normal == sf::Vector3f( 0, 1, 0 ) );
		BOOST_CHECK( vertex.uv == sf::Vector2f( 0, 0 ) );
	}
}
