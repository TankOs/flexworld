#include <FlexWorld/ModelDriver.hpp>
#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Vertex.hpp>
#include <FlexWorld/Math.hpp>
#include <FlexWorld/Cuboid.hpp>
#include <FlexWorld/Face.hpp>

#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#include <SFML/Graphics/Rect.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

static const float AREA_DIFFERENCE_TOLERANCE = 0.001f;

void print_usage() {
	std::cout << "Usage: convert2fwm [OPTIONS...] INPUT OUTPUT" << std::endl
		<< "Convert INPUT model file to OUTPUT FlexWorld Model file." << std::endl
		<< std::endl
		<< "Options:" << std::endl
		<< "  -iv, --invertv           Invert V component of UV coordinates." << std::endl
		<< "  -v, --verbose            Verbose." << std::endl
		<< "  -t, --tolerance          Boundary tolerance. (Default: 0.01)" << std::endl
		<< "  -h, --help               Show this help text." << std::endl
	;
}

void update_coverage( sf::FloatRect& coverage_rect, const sf::Vector2f& min, const sf::Vector2f& max ) {
	// Check if coverage rect hasn't been initialized yet.
	if( coverage_rect.Width == 0 && coverage_rect.Height == 0 ) {
		coverage_rect.Left = min.x;
		coverage_rect.Top = min.y;
		coverage_rect.Width = max.x - min.x;
		coverage_rect.Height = max.y - min.y;
	}
	else {
		// Check if triangle is right of/at the coverage box.
		if( min.x >= coverage_rect.Left ) {
			coverage_rect.Width = std::max( coverage_rect.Width, max.x - coverage_rect.Left );
		}
		else { // Move coverage box and enlarge to cover previous polygons.
			coverage_rect.Width += coverage_rect.Left - min.x;
			coverage_rect.Left -= coverage_rect.Left - min.x;
		}

		// Check if triangle is below of/at the coverage box.
		if( min.y >= coverage_rect.Top ) {
			coverage_rect.Height = std::max( coverage_rect.Height, max.y - coverage_rect.Top );
		}
		else { // Move coverage box and enlarge to cover previous polygons.
			coverage_rect.Height += coverage_rect.Top - min.y;
			coverage_rect.Top -= coverage_rect.Top - min.y;
		}
	}
}


int main( int argc, char** argv ) {
	std::string in_filename = "";
	std::string out_filename = "";
	bool invert_v = false;
	bool verbose = false;
	float boundary_tolerance = 0.01f;

	// Extract commandline arguments.
	for( int arg_index = 1; arg_index < argc; ++arg_index ) {
		std::string arg( argv[arg_index] );

		if( arg[0] == '-' ) {
			if( arg.size() < 2 ) {
				std::cerr << "Missing option. Try --help." << std::endl;
				return -1;
			}

			if( arg == "-h" || arg == "--help" ) { // Help.
				print_usage();
				return 0;
			}
			else if( arg == "-iv" || arg == "--invertv" ) { // Invert V of UV coordinates.
				invert_v = true;
			}
			else if( arg == "-v" || arg == "--verbose" ) { // Verbose.
				verbose = true;
			}
			else if( arg == "-t" || arg == "--tolerance" ) { // Boundary tolerance.
				if( arg_index + 1 >= argc ) {
					std::cerr << "Missing tolerance value." << std::endl;
					return -1;
				}

				++arg_index;
				std::stringstream sstr;
				sstr << argv[arg_index];
				sstr >> boundary_tolerance;

				if( !sstr || boundary_tolerance > 0.5f ) {
					std::cerr << "Invalid boundary tolerance value." << std::endl;
					return -1;
				}
			}
			else {
				std::cerr << "Unrecognized option: " << arg << ". Try --help." << std::endl;
				return -1;
			}
		}
		else {
			if( in_filename.empty() ) {
				in_filename = arg;
			}
			else if( out_filename.empty() ) {
				out_filename = arg;
			}
			else {
				std::cerr << "Too many arguments. Try --help." << std::endl;
				return -1;
			}
		}
	}

	// Check if needed options are present.
	if( in_filename.empty() || out_filename.empty() ) {
		std::cerr << "Missing arguments. Try --help." << std::endl;
		return -1;
	}

	// Load input file.
	using namespace Assimp;

	Importer importer;
	const aiScene* scene( importer.ReadFile( argv[1], aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType ) );

	if( !scene ) {
		std::cerr << "Failed to load " << argv[1] << ". File not found or not supported model format." << std::endl;
		return -1;
	}

	// Check basics.
	if( scene->mNumMeshes < 1 ) {
		std::cerr << "No meshes in the model, aborting." << std::endl;
		return -1;
	}

	//////////////////////////////////////////////////
	// PASS 1:
	//   * Basic error checking.
	//   * Calc shift.
	//   * Calc bounding box.
	//   * Calc block scale divisor.
	//////////////////////////////////////////////////
	sf::Vector3f lowest_point( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
	sf::Vector3f highest_point( 0, 0, 0 );
	sf::Vector3f shift( 0, 0, 0 );
	flex::FloatCuboid bounding_box( 0, 0, 0, 0 );

	for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
		const aiMesh* ai_mesh( scene->mMeshes[mesh_index] );

		// Check for existance of vertices.
		if( ai_mesh->mNumVertices < 3 ) {
			std::cout << "Mesh " << mesh_index << " has less than 3 vertices, skipping." << std::endl;
			continue;
		}

		if( ai_mesh->mNumFaces < 1 ) {
			std::cout << "Mesh " << mesh_index << " doesn't contain any triangles, skipping." << std::endl;
			continue;
		}

		for( unsigned int vertex_index = 0; vertex_index < ai_mesh->mNumVertices; ++vertex_index ) {
			// Check if UV coordinate present.
			if( !ai_mesh->mTextureCoords[0] ) {
				std::cerr << "No UV coordinate for mesh " << mesh_index << ", vertex " << vertex_index << ", aborting." << std::endl;
				return -1;
			}

			// For lazy people.
			sf::Vector3f vec(
				ai_mesh->mVertices[vertex_index].x,
				ai_mesh->mVertices[vertex_index].y,
				ai_mesh->mVertices[vertex_index].z
			);

			// Update lowest point.
			lowest_point.x = std::min( lowest_point.x, vec.x );
			lowest_point.y = std::min( lowest_point.y, vec.y );
			lowest_point.z = std::min( lowest_point.z, vec.z );

			// Update highest point.
			highest_point.x = std::max( highest_point.x, vec.x );
			highest_point.y = std::max( highest_point.y, vec.y );
			highest_point.z = std::max( highest_point.z, vec.z );

			// Calc shift.
			shift.x = std::abs( std::min( shift.x, lowest_point.x ) );
			shift.y = std::abs( std::min( shift.y, lowest_point.y ) );
			shift.z = std::abs( std::min( shift.z, lowest_point.z ) );
		}
	}

	// Add shift to lowest and highest point.
	highest_point += shift;
	lowest_point += shift;

	// Calculate highest value for block scale divisor.
	float scale_divisor = std::max( 1.0f, std::max( highest_point.x, std::max( highest_point.y, highest_point.z ) ) );

	// Calculate bounding box.
	bounding_box.x = lowest_point.x;
	bounding_box.y = lowest_point.y;
	bounding_box.z = lowest_point.z;
	bounding_box.width = highest_point.x - lowest_point.x;
	bounding_box.height = highest_point.y - lowest_point.y;
	bounding_box.depth = highest_point.z - lowest_point.z;

	if(
		bounding_box.x < 0.0f ||
		bounding_box.y < 0.0f ||
		bounding_box.z < 0.0f ||
		bounding_box.width <= 0.0f ||
		bounding_box.height <= 0.0f ||
		bounding_box.depth <= 0.0f
	) {
		std::cerr << "Invalid bounding box." << std::endl;
		return -1;
	}

	//////////////////////////////////////////////////
	// PASS 2: Create model.
	//   * Apply shift.
	//   * Calc coverages.
	//////////////////////////////////////////////////
	std::size_t num_total_vertices = 0;
	std::size_t num_total_triangles = 0;

	std::vector<sf::FloatRect> coverage_rects( flex::NUM_FACES, sf::FloatRect( 0, 0, 0, 0 ) );
	std::vector<float> areas( flex::NUM_FACES, 0 );

	float big_boundary = 1.0f - boundary_tolerance;
	float small_boundary = boundary_tolerance;

	flex::Model model;

	// Add meshes.
	for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
		const aiMesh* ai_mesh( scene->mMeshes[mesh_index] );
		flex::Mesh mesh;

		// Skip empty meshes.
		if( ai_mesh->mNumVertices < 3 || ai_mesh->mNumFaces < 1 ) {
			continue;
		}

		num_total_vertices += ai_mesh->mNumVertices;
		num_total_triangles += ai_mesh->mNumFaces;

		// Extract texture slot.
		std::string material_name;

		{
			// Get material name.
			aiString ai_material_name;
			scene->mMaterials[ai_mesh->mMaterialIndex]->Get( AI_MATKEY_NAME, ai_material_name );
			material_name = ai_material_name.data;
		}

		// Only materials beginning with "slot:" are treated as texture slot definitions.
		if( material_name.substr( 0, 5 ) == "slot:" ) {
			std::stringstream sstr( material_name.substr( 5 ) );
			uint16_t texture_slot( 0 );

			sstr >> texture_slot;
			if( !sstr || texture_slot > 255 ) {
				std::cerr << "Invalid texture slot for mesh " << mesh_index << ", material " << material_name << "." << std::endl;
				return -1;
			}

			mesh.set_texture_slot( static_cast<unsigned char>( texture_slot ) );
		}
		else {
			std::cerr << "Invalid material name for mesh " << mesh_index << ": \"" << material_name << "\". Aborting." << std::endl;
			return -1;
		}

		// Add vertices.
		for( unsigned int vertex_index = 0; vertex_index < ai_mesh->mNumVertices; ++vertex_index ) {
			flex::Vertex vertex(
				sf::Vector3f(
					ai_mesh->mVertices[vertex_index].x + shift.x,
					ai_mesh->mVertices[vertex_index].y + shift.y,
					ai_mesh->mVertices[vertex_index].z + shift.z
				),
				sf::Vector3f(
					ai_mesh->mNormals[vertex_index].x,
					ai_mesh->mNormals[vertex_index].y,
					ai_mesh->mNormals[vertex_index].z
				),
				sf::Vector2f( // Check for inverted coordinates.
					ai_mesh->mTextureCoords[0][vertex_index].x,
					!invert_v ? ai_mesh->mTextureCoords[0][vertex_index].y : 1.f - ai_mesh->mTextureCoords[0][vertex_index].y
				)
			);

			mesh.add_vertex( vertex );
		}

		// Add triangles.
		for( unsigned int triangle_index = 0; triangle_index < ai_mesh->mNumFaces; ++triangle_index ) {
			const aiFace& ai_face( ai_mesh->mFaces[triangle_index] );

			// Enforce triangles. Should be ensured by Assimp triangulate post-processor.
			if( ai_face.mNumIndices != 3 ) {
				std::cerr << "Invalid face in mesh " << mesh_index << ", face " << triangle_index << ": Not a triangle." << std::endl;
				return -1;
			}

			// Check for valid triangle.
			flex::Triangle triangle(
				static_cast<flex::Mesh::TriangleIndex>( ai_face.mIndices[0] ),
				static_cast<flex::Mesh::TriangleIndex>( ai_face.mIndices[1] ),
				static_cast<flex::Mesh::TriangleIndex>( ai_face.mIndices[2] )
			);

			if(
				triangle.vertices[0] >= mesh.get_num_vertices() ||
				triangle.vertices[1] >= mesh.get_num_vertices() ||
				triangle.vertices[2] >= mesh.get_num_vertices() ||
				triangle.vertices[0] == triangle.vertices[1] ||
				triangle.vertices[0] == triangle.vertices[2] ||
				triangle.vertices[1] == triangle.vertices[2]
			) {
				std::cerr << "Invalid vertex index in mesh " << mesh_index << ", face " << triangle_index << ": Index too high or indices equal." << std::endl;
				return -1;
			}

			// Get scaled vertices to calculate coverage.
			const sf::Vector3f& v0 = mesh.get_vertex( triangle.vertices[0] ).position / model.get_block_scale_divisor();
			const sf::Vector3f& v1 = mesh.get_vertex( triangle.vertices[1] ).position / model.get_block_scale_divisor();
			const sf::Vector3f& v2 = mesh.get_vertex( triangle.vertices[2] ).position / model.get_block_scale_divisor();

			const sf::Vector3f& n0 = mesh.get_vertex( triangle.vertices[0] ).normal;
			const sf::Vector3f& n1 = mesh.get_vertex( triangle.vertices[1] ).normal;
			const sf::Vector3f& n2 = mesh.get_vertex( triangle.vertices[2] ).normal;

			// Get low values for each axis.
			sf::Vector3f min(
				std::min( v0.x, std::min( v1.x, v2.x ) ),
				std::min( v0.y, std::min( v1.y, v2.z ) ),
				std::min( v0.z, std::min( v1.z, v2.z ) )
			);

			// Get max values for each axis.
			sf::Vector3f max(
				std::max( v0.x, std::max( v1.x, v2.x ) ),
				std::max( v0.y, std::max( v1.y, v2.z ) ),
				std::max( v0.z, std::max( v1.z, v2.z ) )
			);

			// Update coverage rects.
			if(
				v0.y >= big_boundary && v1.y >= big_boundary && v2.y >= big_boundary &&
				n0.y > 0.0f && n1.y > 0.0f && n2.y > 0.0f
			) {
				areas[flex::UP_FACE] += flex::calc_triangle_area( sf::Vector2f( v0.x, v0.z ), sf::Vector2f( v1.x, v1.z ), sf::Vector2f( v2.x, v2.z ) );
				update_coverage( coverage_rects[flex::UP_FACE], sf::Vector2f( min.x, min.z ), sf::Vector2f( max.x, max.z ) );
			}
			else if(
				v0.y <= small_boundary && v1.y <= small_boundary && v2.y <= small_boundary &&
				n0.y < 0.0f && n1.y < 0.0f && n2.y < 0.0f
			) {
				areas[flex::DOWN_FACE] += flex::calc_triangle_area( sf::Vector2f( v0.x, v0.z ), sf::Vector2f( v1.x, v1.z ), sf::Vector2f( v2.x, v2.z ) );
				update_coverage( coverage_rects[flex::DOWN_FACE], sf::Vector2f( min.x, min.z ), sf::Vector2f( max.x, max.z ) );
			}
			else if(
				v0.z <= small_boundary && v1.z <= small_boundary && v2.z <= small_boundary &&
				n0.z < 0.0f && n1.z < 0.0f && n2.z < 0.0f
			) {
				areas[flex::BACK_FACE] += flex::calc_triangle_area( sf::Vector2f( v0.x, v0.y ), sf::Vector2f( v1.x, v1.y ), sf::Vector2f( v2.x, v2.y ) );
				update_coverage( coverage_rects[flex::BACK_FACE], sf::Vector2f( min.x, min.y ), sf::Vector2f( max.x, max.y ) );
			}
			else if(
				v0.z >= big_boundary && v1.z >= big_boundary && v2.z >= big_boundary &&
				n0.z > 0.0f && n1.z > 0.0f && n2.z > 0.0f
			) {
				areas[flex::FRONT_FACE] += flex::calc_triangle_area( sf::Vector2f( v0.x, v0.y ), sf::Vector2f( v1.x, v1.y ), sf::Vector2f( v2.x, v2.y ) );
				update_coverage( coverage_rects[flex::FRONT_FACE], sf::Vector2f( min.x, min.y ), sf::Vector2f( max.x, max.y ) );
			}
			else if(
				v0.x >= big_boundary && v1.x >= big_boundary && v2.x >= big_boundary &&
				n0.x > 0.0f && n1.x > 0.0f && n2.x > 0.0f
			) {
				areas[flex::RIGHT_FACE] += flex::calc_triangle_area( sf::Vector2f( v0.y, v0.z ), sf::Vector2f( v1.y, v1.z ), sf::Vector2f( v2.y, v2.z ) );
				update_coverage( coverage_rects[flex::RIGHT_FACE], sf::Vector2f( min.y, min.z ), sf::Vector2f( max.y, max.z ) );
			}
			else if(
				v0.x <= small_boundary && v1.x <= small_boundary && v2.x <= small_boundary &&
				n0.x < 0.0f && n1.x < 0.0f && n2.x < 0.0f
			) {
				areas[flex::LEFT_FACE] += flex::calc_triangle_area( sf::Vector2f( v0.y, v0.z ), sf::Vector2f( v1.y, v1.z ), sf::Vector2f( v2.y, v2.z ) );
				update_coverage( coverage_rects[flex::LEFT_FACE], sf::Vector2f( min.y, min.z ), sf::Vector2f( max.y, max.z ) );
			}

			mesh.define_triangle( triangle );
		}

		model.add_mesh( mesh );
	}

	// Reset coverage rects that do not really cover the full rect.
	for( int face_idx = 0; face_idx < flex::NUM_FACES; ++face_idx ) {
		sf::FloatRect& rect = coverage_rects[face_idx];
		float diff = flex::calc_rect_area( rect.Width, rect.Height ) - areas[face_idx];

		if( rect.Width == 0.0f || diff > AREA_DIFFERENCE_TOLERANCE ) {
			rect = sf::FloatRect( 0, 0, 0, 0 );
		}

		// Finally set model's coverage rect.
		model.set_face_coverage( static_cast<flex::Face>( face_idx ), rect );
	}

	// Set basic props.
	model.set_bounding_box( bounding_box );
	model.set_block_scale_divisor( scale_divisor );

	// Write output file.
	flex::ModelDriver::Buffer buffer;
	buffer = flex::ModelDriver::serialize( model );

	std::ofstream out_file( argv[2], std::ios::binary | std::ios::out );

	if( !out_file.is_open() ) {
		std::cerr << "Failed to save FlexWorld Model to " << argv[2] << "." << std::endl;
		return -1;
	}

	out_file.write( &buffer[0], buffer.size() );
	out_file.close();

	// Show a warning if the model got shifted.
	if( shift.x > 0 || shift.y > 0 || shift.z > 0 ) {
		std::cout << std::scientific;

		std::cout
			<< "The source model hasn't got its origin at 0, 0, 0. It has been shifted by "
			<< shift.x << ", " << shift.y << ", " << shift.z
			<< " units."
			<< std::endl
		;
	}

	if( verbose ) {
		// Give some info.
		std::cout
			<< std::fixed
			<< "Model saved to " << argv[2] << "."
			<< std::endl
			<< model.get_num_meshes() << " mesh" << (model.get_num_meshes() > 1 ? "es" : "")
			<< ", " << num_total_vertices << " vertices"
			<< ", " << num_total_triangles << " triangle" << (num_total_triangles > 1 ? "s" : "")
			<< ", block scale divisor " << scale_divisor
			<< ", coverage "
			<< (coverage_rects[flex::UP_FACE].Width > 0.0f ? "U" : "")
			<< (coverage_rects[flex::DOWN_FACE].Width > 0.0f ? "D" : "")
			<< (coverage_rects[flex::BACK_FACE].Width > 0.0f ? "B" : "")
			<< (coverage_rects[flex::RIGHT_FACE].Width > 0.0f ? "R" : "")
			<< (coverage_rects[flex::FRONT_FACE].Width > 0.0f ? "F" : "")
			<< (coverage_rects[flex::LEFT_FACE].Width > 0.0f ? "L" : "")
			<< std::endl
			<< "Bounding box: " << bounding_box.x << ", " << bounding_box.y << ", " << bounding_box.z
			<< " / " << bounding_box.width << " * " << bounding_box.height << " * " << bounding_box.depth
			<< std::endl
		;
	}

	return 0;
}
