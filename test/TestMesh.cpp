#include <FlexWorld/Mesh.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestMesh ) {
	using namespace flex;

	// Initial state.
	{
		Mesh mesh;

		BOOST_CHECK( mesh.get_texture_slot() == 0 );
	}

	// Basic properties.
	{
		Mesh mesh;

		mesh.set_texture_slot( 100 );

		BOOST_CHECK( mesh.get_texture_slot() == 100 );
	}

	// Clear mesh.
	{
		Mesh mesh;

		mesh.set_texture_slot( 3 );

		mesh.get_geometry().add_triangle( sg::Vertex(), sg::Vertex(), sg::Vertex() );
		mesh.get_geometry().add_triangle( sg::Vertex(), sg::Vertex(), sg::Vertex() );
		mesh.get_geometry().add_triangle( sg::Vertex(), sg::Vertex(), sg::Vertex() );

		mesh.clear();

		BOOST_CHECK( mesh.get_geometry().get_num_vertices() == 0 );
		BOOST_CHECK( mesh.get_geometry().get_num_triangles() == 0 );
		BOOST_CHECK( mesh.get_texture_slot() == 0 );
	}
}
