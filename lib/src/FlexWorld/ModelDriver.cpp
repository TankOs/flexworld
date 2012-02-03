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
		for( Mesh::VertexIndex vertex_idx = 0; vertex_idx < mesh.get_num_vertices(); ++vertex_idx ) {
			const Vertex& vertex = mesh.get_vertex( vertex_idx );

			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &vertex ),
				reinterpret_cast<const char*>( &vertex ) + sizeof( vertex )
			);
		}

		// Triangles.
		for( Mesh::TriangleIndex tri_idx = 0; tri_idx < mesh.get_num_triangles(); ++tri_idx ) {
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


}
