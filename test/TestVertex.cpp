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

	// Ctor.
	{
		Vertex vertex(
			sf::Vector3f( 9, 8, 7 ),
			sf::Vector3f( 4, 3, 2 ),
			sf::Vector2f( 22, 33 )
		);

		BOOST_CHECK( vertex.position == sf::Vector3f( 9, 8, 7 ) );
		BOOST_CHECK( vertex.normal == sf::Vector3f( 4, 3, 2 ) );
		BOOST_CHECK( vertex.uv == sf::Vector2f( 22, 33 ) );
	}

	// Test (non-)equality.
	{
		Vertex v0;
		Vertex v1;

		BOOST_CHECK( v0 == v1 );

		v0.position = sf::Vector3f( 1, 2, 3 );
		v0.normal = sf::Vector3f( 10, 11, 12 );
		v0.uv = sf::Vector2f( 21, 22 );

		v1.position = sf::Vector3f( 1, 2, 3 );
		v1.normal = sf::Vector3f( 10, 11, 12 );
		v1.uv = sf::Vector2f( 21, 22 );

		BOOST_CHECK( v0 == v1 );

		v1.position = sf::Vector3f( 0, 0, 0 );
		BOOST_CHECK( v0 != v1 );

		v1.position = sf::Vector3f( 1, 2, 3 );
		v1.normal = sf::Vector3f( 0, 0, 0 );
		BOOST_CHECK( v0 != v1 );

		v1.normal = sf::Vector3f( 10, 11, 12 );
		v1.uv = sf::Vector2f( 0, 0 );
		BOOST_CHECK( v0 != v1 );
	}
}
