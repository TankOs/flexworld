#include <FlexWorld/ModelDriver.hpp>
#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Vertex.hpp>

#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#include <iostream>
#include <sstream>

void print_usage() {
	std::cout << "Usage: convert2fwm [OPTIONS...] INPUT OUTPUT" << std::endl
		<< "Convert INPUT model file to OUTPUT FlexWorld Model file." << std::endl
		<< std::endl
		<< "Options:" << std::endl
		<< "  -a NAME, --author NAME   Set author to NAME." << std::endl
		<< "  -iv, --invertv           Invert V component of UV coordinates." << std::endl
		<< "  -h, --help               Show this help text." << std::endl
	;
}

int main( int argc, char** argv ) {
	std::string in_filename( "" );
	std::string out_filename( "" );
	std::string author( "" );
	bool invert_v( false );

	// Extract commandline arguments.
	for( int arg_index = 1; arg_index < argc; ++arg_index ) {
		std::string arg( argv[arg_index] );

		if( arg[0] == '-' ) {
			if( arg.size() < 2 ) {
				std::cerr << "Missing option. Try --help." << std::endl;
				return -1;
			}

			if( arg == "-a" || arg == "--author" ) { // Author.
				++arg_index;
				if( arg_index >= argc ) {
					std::cerr << "Missing value after author option. Try --help." << std::endl;
					return -1;
				}

				author = argv[arg_index];
			}
			else if( arg == "-h" || arg == "--help" ) { // Help.
				print_usage();
				return 0;
			}
			else if( arg == "-iv" || arg == "--invertv" ) { // Invert V of UV coordinates.
				invert_v = true;
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

	// Check if needed options are present and give info if author setting is missing.
	if( in_filename.empty() || out_filename.empty() ) {
		std::cerr << "Missing arguments. Try --help." << std::endl;
		return -1;
	}

	if( !author.size() ) {
		author = "Unknown";
		std::cout << "Warning: Author set to \"Unknown\". Enter your name using the --author option." << std::endl;
	}

	// Load input file.
	using namespace Assimp;

	Importer importer;
	const aiScene* scene( importer.ReadFile( argv[1], aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType ) );

	if( !scene ) {
		std::cerr << "Failed to load " << argv[1] << ". File not found or not supported model format." << std::endl;
		return -1;
	}

	flex::Model model;
	model.SetAuthor( author );

	// Add meshes.
	for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
		const aiMesh* ai_mesh( scene->mMeshes[mesh_index] );
		flex::Mesh mesh;

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
			unsigned int texture_slot( 0 );

			sstr >> texture_slot;
			if( !sstr || texture_slot > 255 ) {
				std::cerr << "Invalid texture slot for mesh " << mesh_index << ", material " << material_name << "." << std::endl;
				return -1;
			}

			mesh.SetTextureSlot( static_cast<unsigned char>( texture_slot ) );
		}

		// Add vertices.
		for( unsigned int vertex_index = 0; vertex_index < ai_mesh->mNumVertices; ++vertex_index ) {
			// Check if UV coordinate present.
			if( !ai_mesh->mTextureCoords[0] ) {
				std::cerr << "No UV coordinate for mesh " << mesh_index << ", vertex " << vertex_index << ", aborting." << std::endl;
				return -1;
			}

			flex::Vertex vertex(
				sf::Vector3f(
					ai_mesh->mVertices[vertex_index].x,
					ai_mesh->mVertices[vertex_index].y,
					ai_mesh->mVertices[vertex_index].z
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

			mesh.AddVertex( vertex );
		}

		// Add triangles.
		for( unsigned int triangle_index = 0; triangle_index < ai_mesh->mNumFaces; ++triangle_index ) {
			const aiFace& ai_face( ai_mesh->mFaces[triangle_index] );

			// Enforce triangles. Should be ensured by Assimp triangulate post-processor.
			if( ai_face.mNumIndices != 3 ) {
				std::cerr << "Invalid face in mesh " << mesh_index << ", face " << triangle_index << ": Not a triangle." << std::endl;
				return -1;
			}

			mesh.AddTriangle( ai_face.mIndices[0], ai_face.mIndices[1], ai_face.mIndices[2] );
		}

		model.AddMesh( mesh );
	}

	// Write output file.

	if( !flex::ModelDriver::SaveModelToFile( argv[2], model ) ) {
		std::cerr << "Failed to save FlexWorld Model to " << argv[2] << "." << std::endl;
		return -1;
	}

	return 0;
}
