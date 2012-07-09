#include "ColorPicker.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/Entity.hpp>
#include <FlexWorld/World.hpp>

#include <FWSG/Renderer.hpp>
#include <FWSG/TriangleGeometry.hpp>
#include <FWSG/BufferObject.hpp>
#include <FWSG/Transform.hpp>
#include <FWSG/Program.hpp>
#include <FWSG/ProgramCommand.hpp>
#include <map>
#include <memory>
#include <iostream>

/** Ignore.
 * @cond NEVER
 */
struct EntityStepInfo {
	std::list<sg::StepProxy::Ptr> steps;
	sg::Transform transform;
};
// @endcond

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
	const fw::Planet& planet,
	const fw::World& world,
	const std::set<uint32_t>& skip_entity_ids,
	ResourceManager& resource_manager
) {
	static sg::Program::Ptr shader_program;

	// Init shader if not done before.
	if( !shader_program ) {
		shader_program.reset( new sg::Program );

		shader_program->add_shader(
			"uniform vec4 color; void main() { gl_FragColor = color; }",
			sg::Program::FRAGMENT_SHADER
		);

		bool result = shader_program->link();
		assert( result );

		result = shader_program->register_uniform( "color" );
		assert( result );
	}

	// Shoot ray and collect blocks of interest.
	typedef std::map<Result::BlockPosition, const fw::Class*> BlockMap;
	BlockMap blocks;

	float remaining_distance = distance;
	sf::Vector3f current_pos = origin;

	fw::Planet::Vector chunk_pos;
	fw::Chunk::Vector block_pos;

	const fw::Class* block_cls = nullptr;

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
	sf::Color frag_color;

	for( ; block_iter != block_iter_end; ++block_iter ) {
		// Save color -> block mapping and set color.
		color_blocks[color_index] = &block_iter->first;

		// TODO Endianness?
		frag_color.r = static_cast<uint8_t>( (color_index & 0xff0000) >> 16 );
		frag_color.g = static_cast<uint8_t>( (color_index & 0x00ff00) >> 8 );
		frag_color.b = static_cast<uint8_t>( (color_index & 0x0000ff) );

		++color_index;

		// Calc absolute position of block.
		geo_position.x = static_cast<float>( block_iter->first.x );
		geo_position.y = static_cast<float>( block_iter->first.y );
		geo_position.z = static_cast<float>( block_iter->first.z );
		block_cls = block_iter->second;

		// Get or load model.
		const fw::FlexID& model_id = block_cls->get_model().get_id();

		std::shared_ptr<const fw::Model> model = resource_manager.find_model( model_id );

		// If model not found cancel picking. Picking only works on visible data.
		if( !model ) {
			return Result();
		}

		// Iterate over meshes and add geometry.
		std::size_t num_meshes = model->get_num_meshes();
		std::size_t num_triangles = 0;

		for( std::size_t mesh_idx = 0; mesh_idx < num_meshes; ++mesh_idx ) {
			const sg::TriangleGeometry& mesh_geo = model->get_mesh( mesh_idx ).get_geometry();

			num_triangles = mesh_geo.get_num_triangles();

			for( std::size_t triangle_idx = 0; triangle_idx < num_triangles; ++triangle_idx ) {
				sf::Vector3f vectors[3];

				// Get vectors and divide by block scale divisor.
				vectors[0] = mesh_geo.get_vertex( static_cast<uint32_t>( mesh_geo.get_index( triangle_idx * 3 + 0 ) ) ).vector / model->get_block_scale_divisor();
				vectors[1] = mesh_geo.get_vertex( static_cast<uint32_t>( mesh_geo.get_index( triangle_idx * 3 + 1 ) ) ).vector / model->get_block_scale_divisor();
				vectors[2] = mesh_geo.get_vertex( static_cast<uint32_t>( mesh_geo.get_index( triangle_idx * 3 + 2 ) ) ).vector / model->get_block_scale_divisor();

				// Add absolute position.
				vectors[0] += geo_position;
				vectors[1] += geo_position;
				vectors[2] += geo_position;

				geometry.add_triangle(
					sg::Vertex( vectors[0], sf::Vector3f(), sf::Vector2f(), frag_color ),
					sg::Vertex( vectors[1], sf::Vector3f(), sf::Vector2f(), frag_color ),
					sg::Vertex( vectors[2], sf::Vector3f(), sf::Vector2f(), frag_color ),
					false
				);
			}
		}
	}

	// Fetch entities.
	fw::FloatCuboid entity_pick_cuboid(
		origin.x - distance,
		origin.y - distance,
		origin.z - distance,
		distance * 2.0f,
		distance * 2.0f,
		distance * 2.0f
	);

	// Fetch from planet. The array of fetched IDs also works as the color -> ID
	// mapping.
	std::vector<fw::Entity::ID> fetched_entity_ids;

	planet.search_entities( entity_pick_cuboid, fetched_entity_ids );

	// For each entity create a render step with a separate shader program
	// command that sets the appropriate color.
	color_index = static_cast<uint32_t>( color_blocks.size() ); // Reset color index.

	// Create list for holding entity render step infos.
	std::list<EntityStepInfo> ent_render_steps;

	// Create renderer.
	sg::Renderer renderer;
	sg::Transform local_transform;
	std::size_t num_checked_entities = 0;

	for( std::size_t ent_idx = 0; ent_idx < fetched_entity_ids.size(); ++ent_idx ) {
		// Fetch entity.
		const fw::Entity* ent = world.find_entity( fetched_entity_ids[ent_idx] );
		assert( ent );

		if( !ent ) {
			std::cerr << "Color picker retrieved invalid entity." << std::endl;
			return Result();
		}

		// Get buffer object group.
		BufferObjectGroup::PtrConst bo_group = resource_manager.find_buffer_object_group(
			ent->get_class().get_model().get_id()
		);

		// Continue only if the entity has been fully loaded and it's not in the
		// list of to be skipped entities.
		if( bo_group && skip_entity_ids.find( fetched_entity_ids[ent_idx] ) == skip_entity_ids.end() ) {
			++num_checked_entities;

			// Set color.
			frag_color.r = static_cast<uint8_t>( (color_index & 0xff0000) >> 16 );
			frag_color.g = static_cast<uint8_t>( (color_index & 0x00ff00) >> 8 );
			frag_color.b = static_cast<uint8_t>( (color_index & 0x0000ff) );

			// Create the shader program command.
			sg::ProgramCommand::Ptr command( new sg::ProgramCommand( shader_program ) );
			command->set_argument(
				"color",
				static_cast<float>( frag_color.r / 255.0f ),
				static_cast<float>( frag_color.g / 255.0f ),
				static_cast<float>( frag_color.b / 255.0f ),
				1.0f
			);

			// Setup render state.
			sg::RenderState r_state;
			r_state.wireframe = false;
			r_state.depth_test = true;
			r_state.backface_culling = true;
			r_state.program_command = command;

			// Setup entity render step info.
			ent_render_steps.push_back( EntityStepInfo() );

			EntityStepInfo& info = ent_render_steps.back();

			// Setup transform.
			info.transform.set_translation( ent->get_position() );
			info.transform.set_rotation( ent->get_rotation() );
			info.transform.set_origin( ent->get_class().get_origin() );
			info.transform.set_scale( ent->get_class().get_scale() );

			// For each buffer object create a render step.
			for( std::size_t bo_idx = 0; bo_idx < bo_group->get_num_buffer_objects(); ++bo_idx ) {
				sg::StepProxy::Ptr proxy = renderer.create_step(
					r_state,
					transform,
					info.transform,
					bo_group->get_buffer_object( bo_idx )
				);

				info.steps.push_back( proxy );
			}
		}

		++color_index;
	}

	// Create blocks buffer object.
	sg::BufferObject::Ptr bo( new sg::BufferObject( sg::BufferObject::COLORS, false ) );
	bo->load( geometry );

	// Create render state.
	sg::RenderState r_state;
	r_state.wireframe = false;
	r_state.depth_test = true;
	r_state.backface_culling = true;

	// Create blocks render step.
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

	// Release render steps.
	step.reset();
	ent_render_steps.clear();

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

	// Check for invalid color.
	if( read_index >= color_blocks.size() + fetched_entity_ids.size() ) {
		std::cerr << "WARNING: Invalid color index read by picker." << std::endl;
		return Result();
	}
	else if( read_index >= color_blocks.size() ) { // Index belongs to entity.
		Result result;
		result.m_type = Result::ENTITY;
		result.m_entity_id = fetched_entity_ids[read_index - static_cast<uint32_t>( color_blocks.size() )];

		return result;
	}

	// Picked object is a block, so determine clicked face of bounding box in
	// another render pass.
	Result result;
	result.m_type = Result::BLOCK;
	result.m_block_position = *color_blocks[read_index];

	// The final step is to determine the side the user clicked on. This is
	// accomplished by rendering the bounding box.
	std::shared_ptr<const fw::Model> model;

	sf::Vector3f b_pos(
		static_cast<float>( result.m_block_position.x ),
		static_cast<float>( result.m_block_position.y ),
		static_cast<float>( result.m_block_position.z )
	);

	// Get model of hit block.
	{
		bool transform_result = planet.transform( b_pos, chunk_pos, block_pos );
		assert( transform_result );

		block_cls = planet.find_block( chunk_pos, block_pos );
		assert( block_cls );

		model = resource_manager.find_model( block_cls->get_model().get_id() );
		assert( model != nullptr );
	}

	// Build bounding box geometry.
	sg::TriangleGeometry bb_geometry;
	fw::FloatCuboid bb = model->get_bounding_box();

	// Apply block scale divisor.
	bb.x /= model->get_block_scale_divisor();
	bb.y /= model->get_block_scale_divisor();
	bb.z /= model->get_block_scale_divisor();
	bb.width /= model->get_block_scale_divisor();
	bb.height /= model->get_block_scale_divisor();
	bb.depth /= model->get_block_scale_divisor();

	// Front (south face).
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Red ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Red ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Red ),
		false
	);
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Red ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Red ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Red ),
		false
	);

	// Back (north face).
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Green ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Green ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Green ),
		false
	);
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Green ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Green ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Green ),
		false
	);

	// Left (west face).
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Yellow ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Yellow ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Yellow ),
		false
	);
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Yellow ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Yellow ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Yellow ),
		false
	);

	// Right (east face).
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Blue ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Blue ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Blue ),
		false
	);
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Blue ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Blue ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Blue ),
		false
	);

	// Top (up face).
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Black ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Black ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Black ),
		false
	);
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Black ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Black ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y + bb.height, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Black ),
		false
	);

	// Bottom (down face).
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Cyan ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Cyan ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Cyan ),
		false
	);
	bb_geometry.add_triangle(
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Cyan ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z ), sf::Vector3f(), sf::Vector2f(), sf::Color::Cyan ),
		sg::Vertex( sf::Vector3f( b_pos.x + bb.x + bb.width, b_pos.y + bb.y, b_pos.z + bb.z + bb.depth ), sf::Vector3f(), sf::Vector2f(), sf::Color::Cyan ),
		false
	);

	// Render.
	bo.reset( new sg::BufferObject( sg::BufferObject::COLORS, false ) );
	bo->load( bb_geometry );

	step = renderer.create_step(
		r_state,
		transform,
		local_transform,
		bo
	);

	glEnable( GL_SCISSOR_TEST );
	glDisable( GL_LIGHTING );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	renderer.render();

	if( lighting ) {
		glEnable( GL_LIGHTING );
	}

	// Read pixel.
	glReadPixels( pixel_pos.x, pixel_pos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &read_color );

	glDisable( GL_SCISSOR_TEST );

	// If white, which shouldn't happen, nothing got picked.
	if( read_color == sf::Color::White ) {
		//std::cerr << "WARNING: Face picking into the void? Not very clever!" << std::endl;
		return Result();
	}

	if( read_color == sf::Color::Red ) {
		result.m_facing = fw::SOUTH;
	}
	else if( read_color == sf::Color::Green ) {
		result.m_facing = fw::NORTH;
	}
	else if( read_color == sf::Color::Yellow ) {
		result.m_facing = fw::WEST;
	}
	else if( read_color == sf::Color::Blue ) {
		result.m_facing = fw::EAST;
	}
	else if( read_color == sf::Color::Black ) {
		result.m_facing = fw::UP;
	}
	else if( read_color == sf::Color::Cyan ) {
		result.m_facing = fw::DOWN;
	}
	else {
		//std::cerr << "WARNING: Face picker read an invalid color." << std::endl;
		return Result();
	}

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
