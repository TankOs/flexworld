#include <FlexWorld/Mesh.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestMesh ) {
	using namespace flex;

	// Initial state.
	{
		Mesh mesh;

		BOOST_CHECK( mesh.get_num_vertices() == 0 );
		BOOST_CHECK( mesh.get_num_triangles() == 0 );
		BOOST_CHECK( mesh.get_texture_slot() == 0 );
	}

	// Basic properties.
	{
		Mesh mesh;

		mesh.set_texture_slot( 100 );

		BOOST_CHECK( mesh.get_texture_slot() == 100 );
	}

	sg::Vertex source_vertices[3];

	source_vertices[0].vector = sf::Vector3f( 1, 2, 3 );
	source_vertices[0].normal = sf::Vector3f( 4, 5, 6 );
	source_vertices[0].uv = sf::Vector2f( 7, 8 );

	source_vertices[1].vector = sf::Vector3f( 11, 12, 13 );
	source_vertices[1].normal = sf::Vector3f( 14, 15, 16 );
	source_vertices[1].uv = sf::Vector2f( 17, 18 );

	source_vertices[2].vector = sf::Vector3f( 21, 22, 23 );
	source_vertices[2].normal = sf::Vector3f( 24, 25, 26 );
	source_vertices[2].uv = sf::Vector2f( 27, 28 );


	// Add source_vertices.
	{
		Mesh mesh;
		mesh.add_vertex( source_vertices[0] );
		mesh.add_vertex( source_vertices[1] );
		mesh.add_vertex( source_vertices[2] );

		BOOST_CHECK( mesh.get_num_vertices() == 3 );
		BOOST_CHECK( mesh.get_vertex( 0 ) == source_vertices[0] );
		BOOST_CHECK( mesh.get_vertex( 1 ) == source_vertices[1] );
		BOOST_CHECK( mesh.get_vertex( 2 ) == source_vertices[2] );
	}

	// Define triangles.
	{
		Mesh mesh;

		mesh.add_vertex( source_vertices[0] );
		mesh.add_vertex( source_vertices[1] );
		mesh.add_vertex( source_vertices[2] );

		mesh.define_triangle( Triangle( 0, 1, 2 ) );
		mesh.define_triangle( Triangle( 1, 2, 0 ) );
		mesh.define_triangle( Triangle( 2, 0, 1 ) );

		BOOST_CHECK( mesh.get_num_triangles() == 3 );

		BOOST_CHECK( mesh.get_triangle( 0 ) == Triangle( 0, 1, 2 ) );
		BOOST_CHECK( mesh.get_triangle( 1 ) == Triangle( 1, 2, 0 ) );
		BOOST_CHECK( mesh.get_triangle( 2 ) == Triangle( 2, 0, 1 ) );
	}

	// Clear mesh.
	{
		Mesh mesh;

		mesh.set_texture_slot( 3 );

		mesh.add_vertex( source_vertices[0] );
		mesh.add_vertex( source_vertices[1] );
		mesh.add_vertex( source_vertices[2] );

		mesh.define_triangle( Triangle( 0, 1, 2 ) );
		mesh.define_triangle( Triangle( 1, 2, 0 ) );
		mesh.define_triangle( Triangle( 2, 0, 1 ) );

		mesh.clear();

		BOOST_CHECK( mesh.get_num_vertices() == 0 );
		BOOST_CHECK( mesh.get_num_triangles() == 0 );
		BOOST_CHECK( mesh.get_texture_slot() == 0 );
	}
}
