#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Face.hpp>

#include <FWSG/Vertex.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestModel ) {
	using namespace fw;

	// Initial state.
	{
		Model model;

		BOOST_CHECK( model.get_num_meshes() == 0 );
		BOOST_CHECK( model.get_block_scale_divisor() == 1.0f );
		BOOST_CHECK( model.get_bounding_box() == util::FloatCuboid( 0, 0, 0, 0, 0, 0 ) );

		for( std::size_t face_idx = 0; face_idx < NUM_FACES; ++face_idx ) {
			BOOST_CHECK( model.get_face_coverage( static_cast<Face>( face_idx ) ) == sf::FloatRect( 0, 0, 0, 0 ) );
		}
	}

	// Basic properties.
	{
		Model model;

		model.set_block_scale_divisor( 1.5f );
		model.set_bounding_box( util::FloatCuboid( 1, 2, 3, 4, 5, 6 ) );

		BOOST_CHECK( model.get_block_scale_divisor() == 1.5f );
		BOOST_CHECK( model.get_bounding_box() == util::FloatCuboid( 1, 2, 3, 4, 5, 6 ) );
	}

	// Face coverages.
	{
		Model model;
		float runner = 1.0f;

		for( std::size_t face_idx = 0; face_idx < NUM_FACES; ++face_idx ) {
			model.set_face_coverage(
				static_cast<Face>( face_idx ),
				sf::FloatRect(
					runner + 0.0f,
					runner + 1.0f,
					runner + 2.0f,
					runner + 3.0f
				)
			);

			runner += 4.0f;
		}

		// Validate.
		runner = 1.0f;

		for( std::size_t face_idx = 0; face_idx < NUM_FACES; ++face_idx ) {
			BOOST_CHECK(
				model.get_face_coverage( static_cast<Face>( face_idx ) ) == sf::FloatRect(
					runner + 0.0f,
					runner + 1.0f,
					runner + 2.0f,
					runner + 3.0f
				)
			);

			runner += 4.0f;
		}

	}

	// Add meshes.
	{
		Model model;

		Mesh mesh;
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( 1, 1, 1 ), sf::Vector2f( 1, 1 ) ) );
		model.add_mesh( mesh );

		mesh.clear();
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 2, 2, 2 ), sf::Vector3f( 2, 2, 2 ), sf::Vector2f( 2, 2 ) ) );
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 2, 2, 2 ), sf::Vector3f( 2, 2, 2 ), sf::Vector2f( 2, 2 ) ) );
		model.add_mesh( mesh );

		mesh.clear();
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 3, 3, 3 ), sf::Vector3f( 3, 3, 3 ), sf::Vector2f( 3, 3 ) ) );
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 3, 3, 3 ), sf::Vector3f( 3, 3, 3 ), sf::Vector2f( 3, 3 ) ) );
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 3, 3, 3 ), sf::Vector3f( 3, 3, 3 ), sf::Vector2f( 3, 3 ) ) );
		model.add_mesh( mesh );

		mesh.clear();
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		mesh.get_geometry().add_vertex( sg::Vertex( sf::Vector3f( 4, 4, 4 ), sf::Vector3f( 4, 4, 4 ), sf::Vector2f( 4, 4 ) ) );
		model.add_mesh( mesh );

		BOOST_CHECK( model.get_num_meshes() == 4 );

		BOOST_CHECK( model.get_mesh( 0 ).get_geometry().get_num_vertices() == 1 );
		BOOST_CHECK( model.get_mesh( 1 ).get_geometry().get_num_vertices() == 2 );
		BOOST_CHECK( model.get_mesh( 2 ).get_geometry().get_num_vertices() == 3 );
		BOOST_CHECK( model.get_mesh( 3 ).get_geometry().get_num_vertices() == 4 );
	}
}
