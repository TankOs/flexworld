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

	// Bounding box.
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_bounding_box() ), reinterpret_cast<const char*>( &model.get_bounding_box() ) + sizeof( flex::FloatCuboid ) );

	// Coverage rects.
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_face_coverage( UP_FACE ) ), reinterpret_cast<const char*>( &model.get_face_coverage( UP_FACE ) ) + sizeof( sf::FloatRect ) );
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_face_coverage( DOWN_FACE ) ), reinterpret_cast<const char*>( &model.get_face_coverage( DOWN_FACE ) ) + sizeof( sf::FloatRect ) );
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_face_coverage( BACK_FACE ) ), reinterpret_cast<const char*>( &model.get_face_coverage( BACK_FACE ) ) + sizeof( sf::FloatRect ) );
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_face_coverage( RIGHT_FACE ) ), reinterpret_cast<const char*>( &model.get_face_coverage( RIGHT_FACE ) ) + sizeof( sf::FloatRect ) );
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_face_coverage( FRONT_FACE ) ), reinterpret_cast<const char*>( &model.get_face_coverage( FRONT_FACE ) ) + sizeof( sf::FloatRect ) );
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &model.get_face_coverage( LEFT_FACE ) ), reinterpret_cast<const char*>( &model.get_face_coverage( LEFT_FACE ) ) + sizeof( sf::FloatRect ) );

	// Block scale divisor.
	float block_scale_divisor = model.get_block_scale_divisor();
	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &block_scale_divisor ), reinterpret_cast<const char*>( &block_scale_divisor ) + sizeof( block_scale_divisor ) );

	buffer.push_back( static_cast<char>( model.get_num_meshes() ) );

	// Serialize meshes.
	for( std::size_t mesh_idx = 0; mesh_idx < model.get_num_meshes(); ++mesh_idx ) {
		const Mesh& mesh = model.get_mesh( mesh_idx );

		assert( mesh.get_geometry().get_num_vertices() <= std::numeric_limits<uint16_t>::max() );
		assert( mesh.get_geometry().get_num_indices() <= std::numeric_limits<uint16_t>::max() );

		uint16_t num_vertices = static_cast<uint16_t>( mesh.get_geometry().get_num_vertices() );
		uint16_t num_triangles = static_cast<uint16_t>( mesh.get_geometry().get_num_triangles() );

		buffer.insert( buffer.end(), reinterpret_cast<const char*>( &num_vertices ), reinterpret_cast<const char*>( &num_vertices ) + sizeof( num_vertices ) );
		buffer.insert( buffer.end(), reinterpret_cast<const char*>( &num_triangles ), reinterpret_cast<const char*>( &num_triangles ) + sizeof( num_triangles ) );

		buffer.push_back( mesh.get_texture_slot() );

		// Vertices.
		for( uint16_t vertex_idx = 0; vertex_idx < num_vertices; ++vertex_idx ) {
			const sg::Vertex& vertex = mesh.get_geometry().get_vertex( vertex_idx );

			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &vertex.vector ),
				reinterpret_cast<const char*>( &vertex.vector ) + sizeof( vertex.vector )
			);
			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &vertex.normal ),
				reinterpret_cast<const char*>( &vertex.normal ) + sizeof( vertex.normal )
			);
			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &vertex.uv ),
				reinterpret_cast<const char*>( &vertex.uv ) + sizeof( vertex.uv )
			);
		}

		// Triangles/indices.
		for( std::size_t index_idx = 0; index_idx < num_triangles * 3; ++index_idx ) {
			uint16_t index = static_cast<uint16_t>( mesh.get_geometry().get_index( index_idx ) );

			buffer.insert(
				buffer.end(),
				reinterpret_cast<const char*>( &index ),
				reinterpret_cast<const char*>( &index ) + sizeof( index )
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

	// Bounding box.
	flex::FloatCuboid bbox;

	if( buffer.size() - buf_ptr < sizeof( bbox ) ) {
		throw DeserializationException( "Bounding box missing." );
	}

	bbox = *reinterpret_cast<const flex::FloatCuboid*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( bbox );

	if(
		bbox.x < 0.0f || bbox.y < 0.0f || bbox.z < 0.0f ||
		bbox.width <= 0.0f || bbox.height <= 0.0f || bbox.depth <= 0.0f
	) {
		throw DeserializationException( "Wrong bounding box." );
	}

	// Face coverage rects.
	if( buffer.size() - buf_ptr < NUM_FACES * sizeof( sf::FloatRect ) ) {
		throw DeserializationException( "Face coverage rect missing." );
	}

	sf::FloatRect coverage_rects[NUM_FACES];

	for( int face = UP_FACE; face != NUM_FACES; ++face ) {
		coverage_rects[face] = *reinterpret_cast<const sf::FloatRect*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( sf::FloatRect );

		// Check.
		if(
			coverage_rects[face].left < 0.0f ||
			coverage_rects[face].top < 0.0f ||
			coverage_rects[face].width < 0.0f ||
			coverage_rects[face].height < 0.0f
		) {
			throw DeserializationException( "Wrong face coverage rect." );
		}
	}

	// Block scale divisor.
	float block_scale_divisor = 0.0f;

	if( buffer.size() - buf_ptr < sizeof( block_scale_divisor ) ) {
		throw DeserializationException( "Block scale divisor missing." );
	}

	block_scale_divisor = *reinterpret_cast<const float*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( block_scale_divisor );

	if( block_scale_divisor < 1.0f ) {
		throw DeserializationException( "Wrong block scale divisor." );
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

	Model model;

	// Load meshes.
	for( std::size_t mesh_idx = 0; mesh_idx < num_meshes; ++mesh_idx ) {
		uint16_t num_vertices = 0;

		if( buffer.size() - buf_ptr < sizeof( num_vertices ) ) {
			throw DeserializationException( "Vertex count missing." );
		}

		num_vertices = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( num_vertices );

		if( num_vertices < 3 ) {
			throw DeserializationException( "Vertices missing." );
		}

		uint16_t num_triangles = 0;

		if( buffer.size() - buf_ptr < sizeof( num_triangles ) ) {
			throw DeserializationException( "Triangle count missing." );
		}

		num_triangles = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
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

		// Read vertices (vector, normal and UV coordinates only).
		sg::Vertex vertex;

		if( buffer.size() - buf_ptr < num_vertices * (sizeof( vertex.vector ) + sizeof( vertex.normal ) + sizeof( vertex.uv )) ) {
			throw DeserializationException( "Too less vertices." );
		}

		for( uint16_t vertex_idx = 0; vertex_idx < num_vertices; ++vertex_idx ) {
			vertex.vector = *reinterpret_cast<const sf::Vector3f*>( &buffer[buf_ptr] );
			buf_ptr += sizeof( vertex.vector );

			vertex.normal = *reinterpret_cast<const sf::Vector3f*>( &buffer[buf_ptr] );
			buf_ptr += sizeof( vertex.normal );

			vertex.uv = *reinterpret_cast<const sf::Vector2f*>( &buffer[buf_ptr] );
			buf_ptr += sizeof( vertex.uv );

			mesh.get_geometry().add_vertex( vertex );
		}

		// Read triangle indices.
		if( buffer.size() - buf_ptr < num_triangles * sizeof( uint16_t ) * 3 ) {
			throw DeserializationException( "Too less triangles." );
		}

		uint16_t indices[3];

		for( uint16_t triangle_idx = 0; triangle_idx < num_triangles; ++triangle_idx ) {
			indices[0] = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] ); buf_ptr += sizeof( uint16_t );
			indices[1] = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] ); buf_ptr += sizeof( uint16_t );
			indices[2] = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] ); buf_ptr += sizeof( uint16_t );

			// Check for valid triangle.
			if(
				indices[0] >= num_vertices ||
				indices[1] >= num_vertices ||
				indices[2] >= num_vertices ||
				indices[0] == indices[1] ||
				indices[0] == indices[2] ||
				indices[1] == indices[2]
			) {
				throw DeserializationException( "Invalid triangle." );
			}

			mesh.get_geometry().add_index( indices[0] );
			mesh.get_geometry().add_index( indices[1] );
			mesh.get_geometry().add_index( indices[2] );
		}

		model.add_mesh( mesh );
	}

	// Set basic props.
	model.set_bounding_box( bbox );
	model.set_block_scale_divisor( block_scale_divisor );

	// Set face coverage rects.
	for( int face = UP_FACE; face != NUM_FACES; ++face ) {
		model.set_face_coverage( static_cast<Face>( face ), coverage_rects[face] );
	}

	return model;
}

}
