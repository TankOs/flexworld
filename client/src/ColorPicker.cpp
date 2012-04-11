#include "ColorPicker.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Class.hpp>

#include <FWSG/Renderer.hpp>
#include <FWSG/TriangleGeometry.hpp>
#include <FWSG/BufferObject.hpp>
#include <FWSG/Transform.hpp>
#include <map>
#include <iostream>

namespace sf {

bool operator<( const ColorPicker::Result::BlockPosition& first, const ColorPicker::Result::BlockPosition& second ) {
	if( first.z < second.z ) {
		return true;
	}
	else if( first.z > second.z ) {
		return false;
	}

	if( first.y < second.y ) {
		return true;
	}
	else if( first.y > second.y ) {
		return false;
	}

	if( first.x < second.x ) {
		return true;
	}

	return false;
}

}

ColorPicker::Result ColorPicker::pick(
	const sf::Vector3f& origin,
	const sf::Vector3f forward,
	float distance,
	const sg::Transform& transform,
	const sf::Vector2i& pixel_pos,
	const flex::Planet& planet,
	ResourceManager& resource_manager
) {
	// Shoot ray and collect blocks of interest.
	typedef std::map<Result::BlockPosition, const flex::Class*> BlockMap;
	BlockMap blocks;

	float remaining_distance = distance;
	sf::Vector3f current_pos = origin;

	flex::Planet::Vector chunk_pos;
	flex::Chunk::Vector block_pos;

	const flex::Class* block_cls = nullptr;

	Result::BlockPosition abs_pos;
	Result::BlockPosition old_abs_pos;
	bool first = true;

	while( remaining_distance > 0.0f ) {
		// Save absolute position.
		abs_pos.x = static_cast<uint32_t>( current_pos.x );
		abs_pos.y = static_cast<uint32_t>( current_pos.y );
		abs_pos.z = static_cast<uint32_t>( current_pos.z );

		// Transform planet coordinate to chunk/block coordinates.
		if(
			planet.transform( current_pos, chunk_pos, block_pos ) &&
			planet.has_chunk( chunk_pos ) &&
			(block_cls = planet.find_block( chunk_pos, block_pos )) != nullptr
		) {
			blocks[abs_pos] = block_cls;
		}

		// If this isn't the first check then compare the new position against the
		// old one. If a block step on one axis changed and other axes changed too,
		// then add the proper block inbetween. This creates correct lines.
		// Otherwise blocks may be skipped due to too large distance steps.
		if( !first ) {
			std::vector<Result::BlockPosition> extra_blocks;

			if( abs_pos.x != old_abs_pos.x && abs_pos.y != old_abs_pos.y && abs_pos.z != old_abs_pos.z ) {
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, abs_pos.y, old_abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( old_abs_pos.x, abs_pos.y, old_abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, old_abs_pos.y, old_abs_pos.z ) );

				extra_blocks.push_back( Result::BlockPosition( old_abs_pos.x, abs_pos.y, abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, old_abs_pos.y, abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( old_abs_pos.x, old_abs_pos.y, abs_pos.z ) );
			}
			else if( abs_pos.x != old_abs_pos.x && abs_pos.y != old_abs_pos.y ) {
				extra_blocks.push_back( Result::BlockPosition( old_abs_pos.x, abs_pos.y, abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, old_abs_pos.y, abs_pos.z ) );
			}
			else if( abs_pos.x != old_abs_pos.x && abs_pos.z != old_abs_pos.z ) {
				extra_blocks.push_back( Result::BlockPosition( old_abs_pos.x, abs_pos.y, abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, abs_pos.y, old_abs_pos.z ) );
			}
			else if( abs_pos.y != old_abs_pos.y && abs_pos.z != old_abs_pos.z ) {
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, old_abs_pos.y, abs_pos.z ) );
				extra_blocks.push_back( Result::BlockPosition( abs_pos.x, abs_pos.y, old_abs_pos.z ) );
			}

			// Check extra blocks and add to set.
			for( std::size_t idx = 0; idx < extra_blocks.size(); ++idx ) {
				const Result::BlockPosition& extra_pos = extra_blocks[idx];

				if(
					planet.transform(
						sf::Vector3f(
							static_cast<float>( extra_pos.x ),
							static_cast<float>( extra_pos.y ),
							static_cast<float>( extra_pos.z )
						),
						chunk_pos,
						block_pos
					) &&
					planet.has_chunk( chunk_pos ) &&
					(block_cls = planet.find_block( chunk_pos, block_pos )) != nullptr
				) {
					blocks[extra_pos] = block_cls;
				}
			}
		}

		old_abs_pos = abs_pos;

		current_pos.x += forward.x;
		current_pos.y += forward.y;
		current_pos.z += forward.z;

		remaining_distance -= 1.0f;
		first = false;
	}

	// Create geometry and color->block mappings.
	sg::TriangleGeometry geometry;

	BlockMap::iterator block_iter( blocks.begin() );
	BlockMap::iterator block_iter_end( blocks.end() );
	sf::Vector3f geo_position;

	std::vector<const Result::BlockPosition*> color_blocks( blocks.size() );
	uint32_t color_index = 0;
	sf::Color block_color;

	for( ; block_iter != block_iter_end; ++block_iter ) {
		// Save color -> block mapping and set color.
		color_blocks[color_index] = &block_iter->first;

		// TODO Endianness?
		block_color.r = static_cast<uint8_t>( (color_index & 0xff0000) >> 16 );
		block_color.g = static_cast<uint8_t>( (color_index & 0x00ff00) >> 8 );
		block_color.b = static_cast<uint8_t>( (color_index & 0x0000ff) );

		++color_index;

		// Calc absolute position of block.
		geo_position.x = static_cast<float>( block_iter->first.x );
		geo_position.y = static_cast<float>( block_iter->first.y );
		geo_position.z = static_cast<float>( block_iter->first.z );
		block_cls = block_iter->second;

		// Get or load model.
		const flex::FlexID& model_id = block_cls->get_model().get_id();

		std::shared_ptr<const flex::Model> model = resource_manager.find_model( model_id );

		// If not found try to load.
		if( !model ) {
			if( !resource_manager.load_model( model_id ) ) {
				std::cerr << "ERROR: Failed to load model " << model_id.get() << " (color picker)." << std::endl;
				return Result();
			}

			model = resource_manager.find_model( model_id );
			assert( model != nullptr );
		}

		// Iterate over meshes and add geometry.
		std::size_t num_meshes = model->get_num_meshes();
		std::size_t num_triangles = 0;

		for( std::size_t mesh_idx = 0; mesh_idx < num_meshes; ++mesh_idx ) {
			const sg::TriangleGeometry& mesh_geo = model->get_mesh( mesh_idx ).get_geometry();

			num_triangles = mesh_geo.get_num_triangles();

			for( std::size_t triangle_idx = 0; triangle_idx < num_triangles; ++triangle_idx ) {
				sf::Vector3f vectors[3];

				vectors[0] = geo_position + mesh_geo.get_vertex( static_cast<uint32_t>( mesh_geo.get_index( triangle_idx * 3 + 0 ) ) ).vector;
				vectors[1] = geo_position + mesh_geo.get_vertex( static_cast<uint32_t>( mesh_geo.get_index( triangle_idx * 3 + 1 ) ) ).vector;
				vectors[2] = geo_position + mesh_geo.get_vertex( static_cast<uint32_t>( mesh_geo.get_index( triangle_idx * 3 + 2 ) ) ).vector;

				geometry.add_triangle(
					sg::Vertex( vectors[0], sf::Vector3f(), sf::Vector2f(), block_color ),
					sg::Vertex( vectors[1], sf::Vector3f(), sf::Vector2f(), block_color ),
					sg::Vertex( vectors[2], sf::Vector3f(), sf::Vector2f(), block_color ),
					false
				);
			}
		}
	}

	// Create buffer object.
	sg::BufferObject::Ptr bo( new sg::BufferObject( sg::BufferObject::COLORS, false ) );
	bo->load( geometry );

	// Create render state.
	sg::RenderState r_state;
	r_state.wireframe = false;
	r_state.depth_test = true;
	r_state.backface_culling = true;

	// Create renderer and step.
	sg::Renderer renderer;
	sg::Transform local_transform;

	sg::StepProxy::Ptr step = renderer.create_step(
		r_state,
		transform,
		local_transform,
		bo
	);

	// Save current OpenGL options.
	bool lighting = glIsEnabled( GL_LIGHTING );

	// Setup OpenGL.
	glDisable( GL_LIGHTING );

	glScissor( pixel_pos.x, pixel_pos.y, 1, 1 );
	glEnable( GL_SCISSOR_TEST );

	// Render.
	glClearColor( 1, 1, 1, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	renderer.render();

	// Read pixel.
	sf::Color read_color;

	glReadPixels( pixel_pos.x, pixel_pos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &read_color );

	// Restore OpenGL options.
	glDisable( GL_SCISSOR_TEST );

	if( lighting ) {
		glEnable( GL_LIGHTING );
	}


	// Check if background (= nothing) picked.
	if( read_color == sf::Color::White ) {
		return Result();
	}

	// Restore index.
	uint32_t read_index =
		read_color.r * 0x010000 +
		read_color.g * 0x000100 +
		read_color.b * 0x000001
	;

	if( read_index >= color_blocks.size() ) {
		std::cerr << "WARNING: Invalid color index read by picker." << std::endl;
		return Result();
	}

	Result result;
	result.m_type = Result::BLOCK;
	result.m_block_position = *color_blocks[read_index];

	return result;
}

ColorPicker::ColorPicker() {
}

ColorPicker::ColorPicker( const ColorPicker& /*other*/ ) {
}

ColorPicker::Result::Result() :
	m_type( NONE ),
	m_block_position( 0, 0, 0 ),
	m_entity_id( 0 )
{
}
