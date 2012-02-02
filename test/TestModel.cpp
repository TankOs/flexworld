#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Vertex.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestModel ) {
	using namespace flex;

	// Initial state.
	{
		Model model;

		BOOST_CHECK( model.get_num_meshes() == 0 );
	}

	// Add meshes.
	{
		Model model;

		Mesh mesh;
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( 1, 1, 1 ), sf::Vector2f( 1, 1 ) ) );
		model.add_mesh( mesh );

		mesh.clear();
		mesh.add_vertex( Vertex( sf::Vector3f( 2, 2, 2 ), sf::Vector3f( 2, 2, 2 ), sf::Vector2f( 2, 2 ) ) );
		mesh.add_vertex( Vertex( sf::Vector3f( 2, 2, 2 ), sf::Vector3f( 2, 2, 2 ), sf::Vector2f( 2, 2 ) ) );
		model.add_mesh( mesh );

		mesh.clear();
		mesh.add_vertex( Vertex( sf::Vector3f( 3, 3, 3 ), sf::Vector3f( 3, 3, 3 ), sf::Vector2f( 3, 3 ) ) );
		mesh.add_vertex( Vertex( sf::Vector3f( 3, 3, 3 ), sf::Vector3f( 3, 3, 3 ), sf::Vector2f( 3, 3 ) ) );
		mesh.add_vertex( Vertex( sf::Vector3f( 3, 3, 3 ), sf::Vector3f( 3, 3, 3 ), sf::Vector2f( 3, 3 ) ) );
		model.add_mesh( mesh );

		mesh.clear();
		mesh.add_vertex( Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		mesh.add_vertex( Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		mesh.add_vertex( Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		mesh.add_vertex( Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		model.add_mesh( mesh );

		BOOST_CHECK( model.get_num_meshes() == 4 );

		BOOST_CHECK( model.get_mesh( 0 ).get_num_vertices() == 1 );
		BOOST_CHECK( model.get_mesh( 1 ).get_num_vertices() == 2 );
		BOOST_CHECK( model.get_mesh( 2 ).get_num_vertices() == 3 );
		BOOST_CHECK( model.get_mesh( 3 ).get_num_vertices() == 4 );
	}
}
