#include <FlexWorld/ModelDriver.hpp>
#include <FlexWorld/Model.hpp>

#include <limits>
#include <cstdint>
#include <cassert>

namespace flex {

ModelDriver::Buffer ModelDriver::serialize( const Model& model ) {
	assert( model.get_num_meshes() < 256 );

	Buffer buffer;

	buffer.push_back( 'F' );
	buffer.push_back( 'W' );
	buffer.push_back( 'M' );
	buffer.push_back( 0x00 ); // Version.
	buffer.push_back( static_cast<char>( model.get_num_meshes() ) );

	// Serialize meshes.
	for( std::size_t mesh_idx = 0; mesh_idx < model.get_num_meshes(); ++mesh_idx ) {
		const Mesh& mesh = model.get_mesh( mesh_idx );
		assert( mesh.get_num_vertices() <= std::numeric_limits<Mesh::VertexIndex>::max() );

		Mesh::VertexIndex num_vertices = static_cast<Mesh::VertexIndex>( mesh.get_num_vertices() );
		Mesh::TriangleIndex num_triangles = static_cast<Mesh::TriangleIndex>( mesh.get_num_triangles() );

		buffer.insert( buffer.end(), reinterpret_cast<const char*>( &num_vertices ), reinterpret_cast<const char*>( &num_vertices ) + sizeof( num_vertices ) );
		buffer.insert( buffer.end(), reinterpret_cast<const char*>( &num_triangles ), reinterpret_cast<const char*>( &num_triangles ) + sizeof( num_triangles ) );
		buffer.push_back( mesh.get_texture_slot() );

		// Vertices.
		for( Mesh::VertexIndex vertex_idx = 0; vertex_idx < num_vertices; ++vertex_idx ) {
			const Vertex& vertex = mesh.get_vertex( vertex_idx );

			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &vertex ),
				reinterpret_cast<const char*>( &vertex ) + sizeof( vertex )
			);
		}

		// Triangles.
		for( Mesh::TriangleIndex tri_idx = 0; tri_idx < num_triangles; ++tri_idx ) {
			const Triangle& triangle = mesh.get_triangle( tri_idx );

			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &triangle ),
				reinterpret_cast<const char*>( &triangle ) + sizeof( triangle )
			);
		}
	}

	return buffer;
}

Model ModelDriver::deserialize( const Buffer& buffer ) {
	std::size_t buf_ptr = 0;

	// Identifier.
	if( buffer.size() - buf_ptr < 3 ) {
		throw DeserializationException( "Identifier missing." );
	}

	const char* identifier_ptr = &buffer[buf_ptr];
	buf_ptr += 3;

	if(
		identifier_ptr[0] != 'F' ||
		identifier_ptr[1] != 'W' ||
		identifier_ptr[2] != 'M'
	) {
		throw DeserializationException( "Wrong identifier." );
	}

	// Version.
	unsigned char version = 0;

	if( buffer.size() - buf_ptr < sizeof( version ) ) {
		throw DeserializationException( "Version missing." );
	}

	version = buffer[buf_ptr];
	buf_ptr += sizeof( version );

	if( version != 0 ) {
		throw DeserializationException( "Wrong version." );
	}

	// Mesh count.
	unsigned char num_meshes = 0;

	if( buffer.size() - buf_ptr < sizeof( num_meshes ) ) {
		throw DeserializationException( "Mesh count missing." );
	}

	num_meshes = buffer[buf_ptr];
	buf_ptr += sizeof( num_meshes );

	if( num_meshes < 1 ) {
		throw DeserializationException( "Meshes missing." );
	}

	// Load meshes.
	Model model;

	for( std::size_t mesh_idx = 0; mesh_idx < num_meshes; ++mesh_idx ) {
		Mesh::VertexIndex num_vertices;

		if( buffer.size() - buf_ptr < sizeof( num_vertices ) ) {
			throw DeserializationException( "Vertex count missing." );
		}

		num_vertices = *reinterpret_cast<const Mesh::VertexIndex*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( num_vertices );

		if( num_vertices < 1 ) {
			throw DeserializationException( "Vertices missing." );
		}

		Mesh::TriangleIndex num_triangles;

		if( buffer.size() - buf_ptr < sizeof( num_triangles ) ) {
			throw DeserializationException( "Triangle count missing." );
		}

		num_triangles = *reinterpret_cast<const Mesh::TriangleIndex*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( num_triangles );

		if( num_triangles < 1 ) {
			throw DeserializationException( "Triangles missing." );
		}

		Mesh::TextureSlot texture_slot;

		if( buffer.size() - buf_ptr < sizeof( texture_slot ) ) {
			throw DeserializationException( "Texture slot missing." );
		}

		texture_slot = *reinterpret_cast<const Mesh::TextureSlot*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( texture_slot );

		Mesh mesh;
		mesh.set_texture_slot( texture_slot );

		// Read vertices.
		Vertex vertex;

		if( buffer.size() - buf_ptr < num_vertices * sizeof( vertex ) ) {
			throw DeserializationException( "Too less vertices." );
		}

		for( std::size_t vertex_idx = 0; vertex_idx < num_vertices; ++vertex_idx ) {
			vertex = *reinterpret_cast<const Vertex*>( &buffer[buf_ptr] );
			buf_ptr += sizeof( vertex );

			mesh.add_vertex( vertex );
		}

		// Read triangles.
		Triangle triangle;

		if( buffer.size() - buf_ptr < num_triangles * sizeof( triangle ) ) {
			throw DeserializationException( "Too less triangles." );
		}

		for( std::size_t triangle_idx = 0; triangle_idx < num_triangles; ++triangle_idx ) {
			triangle = *reinterpret_cast<const Triangle*>( &buffer[buf_ptr] );
			buf_ptr += sizeof( triangle );

			// Check for valid triangle.
			if(
				triangle.vertices[0] >= num_vertices ||
				triangle.vertices[1] >= num_vertices ||
				triangle.vertices[2] >= num_vertices ||
				triangle.vertices[0] == triangle.vertices[1] ||
				triangle.vertices[0] == triangle.vertices[2] ||
				triangle.vertices[1] == triangle.vertices[2]
			) {
				throw DeserializationException( "Invalid triangle." );
			}

			mesh.define_triangle( triangle );
		}

		model.add_mesh( mesh );
	}

	return model;
}

}
