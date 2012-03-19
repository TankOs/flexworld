#include "PlanetDrawable.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Model.hpp>
#include <FlexWorld/Class.hpp>

#include <FWSG/BufferObject.hpp>
#include <FWSG/TriangleGeometry.hpp>
#include <FWSG/Renderer.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

static const float GRID_SIZE = 1.0f;
static const float BOUNDARY_TOLERANCE = 0.001f;

inline bool is_triangle_covered(
	float v0x, float v0y,
	float v1x, float v1y,
	float v2x, float v2y,
	const sf::FloatRect& rect
) {
	return
		(
			v0x + BOUNDARY_TOLERANCE >= rect.left &&
			v0y + BOUNDARY_TOLERANCE >= rect.top &&
			v0x - BOUNDARY_TOLERANCE <= rect.left + rect.width &&
			v0y - BOUNDARY_TOLERANCE <= rect.top + rect.height
		) &&
		(
			v1x + BOUNDARY_TOLERANCE >= rect.left &&
			v1y + BOUNDARY_TOLERANCE >= rect.top &&
			v1x - BOUNDARY_TOLERANCE <= rect.left + rect.width &&
			v1y - BOUNDARY_TOLERANCE <= rect.top + rect.height
		) &&
		(
			v2x + BOUNDARY_TOLERANCE >= rect.left &&
			v2y + BOUNDARY_TOLERANCE >= rect.top &&
			v2x - BOUNDARY_TOLERANCE <= rect.left + rect.width &&
			v2y - BOUNDARY_TOLERANCE <= rect.top + rect.height
		)
	;
}



PlanetDrawable::~PlanetDrawable() {
}

PlanetDrawable::Ptr PlanetDrawable::create( const flex::Planet& planet, ResourceManager& resource_manager, sg::Renderer& renderer ) {
	Ptr planet_drw( new PlanetDrawable( planet, resource_manager, renderer ) );
	return planet_drw;
}

PlanetDrawable::PlanetDrawable( const flex::Planet& planet, ResourceManager& resource_manager, sg::Renderer& renderer ) :
	Drawable( renderer ),
	m_planet( &planet ),
	m_resource_manager( resource_manager ),
	m_view_radius( 12 )
{
}

void PlanetDrawable::set_view_radius( flex::Planet::ScalarType radius ) {
	m_view_radius = radius;
}

void PlanetDrawable::prepare_chunk( const flex::Planet::Vector& chunk_pos ) {
	typedef std::map<std::shared_ptr<const sf::Texture>, std::shared_ptr<sg::TriangleGeometry>> TextureGeometryMap;
	const flex::Chunk::Vector& chunk_size = m_planet->get_chunk_size();

	sf::Vector3f org_offset(
		static_cast<float>( chunk_pos.x * chunk_size.x ) * GRID_SIZE,
		static_cast<float>( chunk_pos.y * chunk_size.y ) * GRID_SIZE,
		static_cast<float>( chunk_pos.z * chunk_size.z ) * GRID_SIZE
	);
	sf::Vector3f offset = org_offset;

	const flex::Class* block_cls = nullptr;
	const flex::Class* nbor_block_cls = nullptr;
	std::shared_ptr<const flex::Model> model;
	std::shared_ptr<const flex::Model> nbor_model;
	std::shared_ptr<const sf::Texture> texture;
	TextureGeometryMap new_geometries;
	sg::Vertex vertex[3];
	const sf::FloatRect* coverage_rects[flex::NUM_FACES];
	flex::Chunk::Vector block_runner;

	for( block_runner.z = 0; block_runner.z < chunk_size.z; ++block_runner.z ) {
		for( block_runner.y = 0; block_runner.y < chunk_size.y; ++block_runner.y ) {
			for( block_runner.x = 0; block_runner.x < chunk_size.x; ++block_runner.x ) {
				// Skip empty blocks.
				block_cls = m_planet->find_block( chunk_pos, block_runner );

				if( block_cls != nullptr ) {
					// Get model.
					model = get_model( block_cls->get_model().get_id() );

					assert( model );
					if( !model ) {
						// TODO Error handling.
						return;
					}

					////////////////////////////////////////////////////////////////
					///////////// COVERAGE PREPARATION /////////////////////////////
					////////////////////////////////////////////////////////////////
					//
					// Get all neighbour's coverage rects.

					coverage_rects[flex::UP_FACE] =
						(
							block_runner.y + 1 < m_planet->get_chunk_size().y &&
							(nbor_block_cls = m_planet->find_block( chunk_pos, block_runner + flex::Chunk::Vector( 0, 1, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::DOWN_FACE )
						: 0
					;

					coverage_rects[flex::DOWN_FACE] =
						(
							block_runner.y > 0 &&
							(nbor_block_cls = m_planet->find_block( chunk_pos, block_runner - flex::Chunk::Vector( 0, 1, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::UP_FACE )
						: 0
					;

					coverage_rects[flex::LEFT_FACE] =
						(
							block_runner.x > 0 &&
							(nbor_block_cls = m_planet->find_block( chunk_pos, block_runner - flex::Chunk::Vector( 1, 0, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::RIGHT_FACE )
						: 0
					;

					coverage_rects[flex::RIGHT_FACE] =
						(
							block_runner.x + 1 < m_planet->get_chunk_size().x &&
							(nbor_block_cls = m_planet->find_block( chunk_pos, block_runner + flex::Chunk::Vector( 1, 0, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::LEFT_FACE )
						: 0
					;

					coverage_rects[flex::FRONT_FACE] =
						(
							block_runner.z + 1 < m_planet->get_chunk_size().z &&
							(nbor_block_cls = m_planet->find_block( chunk_pos, block_runner + flex::Chunk::Vector( 0, 0, 1 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::BACK_FACE )
						: 0
					;

					coverage_rects[flex::BACK_FACE] =
						(
							block_runner.z > 0 &&
							(nbor_block_cls = m_planet->find_block( chunk_pos, block_runner - flex::Chunk::Vector( 0, 0, 1 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::FRONT_FACE )
						: 0
					;

					/////////////////////////////////////////////////////////////////
					////////////// COVERAGE RECT PREPARATION DONE ///////////////////
					/////////////////////////////////////////////////////////////////

					// Iterate over meshes.
					for( std::size_t mesh_idx = 0; mesh_idx < model->get_num_meshes(); ++mesh_idx ) {
						const flex::Mesh& mesh = model->get_mesh( mesh_idx );

						assert( mesh.get_texture_slot() < block_cls->get_num_textures() );

						// Get texture.
						texture = get_texture( block_cls->get_texture( mesh.get_texture_slot() ).get_id() );

						assert( texture );
						if( !texture ) {
							// TODO Error handling.
							return;
						}

						// Get geometry or generate a new one.
						std::shared_ptr<sg::TriangleGeometry>& geometry = new_geometries[texture];

						if( geometry == nullptr ) {
							geometry.reset( new sg::TriangleGeometry );
						}

						// Iterate over triangles.
						for( uint32_t tri_idx = 0; tri_idx < mesh.get_geometry().get_num_triangles(); ++tri_idx ) {
							uint16_t indices[3];

							indices[0] = static_cast<uint16_t>( mesh.get_geometry().get_index( (tri_idx * 3) + 0 ) );
							indices[1] = static_cast<uint16_t>( mesh.get_geometry().get_index( (tri_idx * 3) + 1 ) );
							indices[2] = static_cast<uint16_t>( mesh.get_geometry().get_index( (tri_idx * 3) + 2 ) );

							// Get triangle's vertices.
							vertex[0] = mesh.get_geometry().get_vertex( indices[0] );
							vertex[1] = mesh.get_geometry().get_vertex( indices[1] );
							vertex[2] = mesh.get_geometry().get_vertex( indices[2] );

							// Apply block scale divisor.
							vertex[0].vector /= model->get_block_scale_divisor();
							vertex[1].vector /= model->get_block_scale_divisor();
							vertex[2].vector /= model->get_block_scale_divisor();

							/////// Hidden-face-removal.
							if(
								// left face.
								(
									coverage_rects[flex::LEFT_FACE] &&
									vertex[0].vector.x <= BOUNDARY_TOLERANCE &&
									vertex[1].vector.x <= BOUNDARY_TOLERANCE &&
									vertex[2].vector.x <= BOUNDARY_TOLERANCE &&
									is_triangle_covered(
										vertex[0].vector.y,
										vertex[0].vector.z,
										vertex[1].vector.y,
										vertex[1].vector.z,
										vertex[2].vector.y,
										vertex[2].vector.z,
										*coverage_rects[flex::LEFT_FACE]
									)
								)
								||
								// Right face.
								(
									coverage_rects[flex::RIGHT_FACE] &&
									vertex[0].vector.x >= 1.0f - BOUNDARY_TOLERANCE &&
									vertex[1].vector.x >= 1.0f - BOUNDARY_TOLERANCE &&
									vertex[2].vector.x >= 1.0f - BOUNDARY_TOLERANCE &&
									is_triangle_covered(
										vertex[0].vector.y,
										vertex[0].vector.z,
										vertex[1].vector.y,
										vertex[1].vector.z,
										vertex[2].vector.y,
										vertex[2].vector.z,
										*coverage_rects[flex::RIGHT_FACE]
									)
								)
								||
								// Up face.
								(
									coverage_rects[flex::UP_FACE] &&
									vertex[0].vector.y >= 1.0f - BOUNDARY_TOLERANCE &&
									vertex[1].vector.y >= 1.0f - BOUNDARY_TOLERANCE &&
									vertex[2].vector.y >= 1.0f - BOUNDARY_TOLERANCE &&
									is_triangle_covered(
										vertex[0].vector.x,
										vertex[0].vector.z,
										vertex[1].vector.x,
										vertex[1].vector.z,
										vertex[2].vector.x,
										vertex[2].vector.z,
										*coverage_rects[flex::UP_FACE]
									)
								)
								||
								// Down face.
								(
									coverage_rects[flex::DOWN_FACE] &&
									vertex[0].vector.y <= BOUNDARY_TOLERANCE &&
									vertex[1].vector.y <= BOUNDARY_TOLERANCE &&
									vertex[2].vector.y <= BOUNDARY_TOLERANCE &&
									is_triangle_covered(
										vertex[0].vector.x,
										vertex[0].vector.z,
										vertex[1].vector.x,
										vertex[1].vector.z,
										vertex[2].vector.x,
										vertex[2].vector.z,
										*coverage_rects[flex::DOWN_FACE]
									)
								)
								||
								// Front face.
								(
									coverage_rects[flex::FRONT_FACE] &&
									vertex[0].vector.z >= 1.0f - BOUNDARY_TOLERANCE &&
									vertex[1].vector.z >= 1.0f - BOUNDARY_TOLERANCE &&
									vertex[2].vector.z >= 1.0f - BOUNDARY_TOLERANCE &&
									is_triangle_covered(
										vertex[0].vector.x,
										vertex[0].vector.y,
										vertex[1].vector.x,
										vertex[1].vector.y,
										vertex[2].vector.x,
										vertex[2].vector.y,
										*coverage_rects[flex::FRONT_FACE]
									)
								)
								||
								// Back face.
								(
									coverage_rects[flex::BACK_FACE] &&
									vertex[0].vector.z <= BOUNDARY_TOLERANCE &&
									vertex[1].vector.z <= BOUNDARY_TOLERANCE &&
									vertex[2].vector.z <= BOUNDARY_TOLERANCE &&
									is_triangle_covered(
										vertex[0].vector.x,
										vertex[0].vector.y,
										vertex[1].vector.x,
										vertex[1].vector.y,
										vertex[2].vector.x,
										vertex[2].vector.y,
										*coverage_rects[flex::BACK_FACE]
									)
								)
							) {
								// Triangle is covered, skip it.
								continue;
							}

							// Apply world position.
							vertex[0].vector += offset;
							vertex[1].vector += offset;
							vertex[2].vector += offset;

							geometry->add_triangle( vertex[0], vertex[1], vertex[2], false );
						}
					}
				}

				offset.x += GRID_SIZE;
			}

			offset.y += GRID_SIZE;
			offset.x = org_offset.x;
		}

		offset.z += GRID_SIZE;
		offset.y = org_offset.y;
	}

	// Prepare (delayed) renderer step.
	DelayedStep delayed;
	delayed.chunk_pos = (
		chunk_pos.z * (m_planet->get_size().x * m_planet->get_size().y) +
		chunk_pos.y * (m_planet->get_size().x) +
		chunk_pos.x
	);

	TextureGeometryMap::iterator tex_geometry_iter( new_geometries.begin() );
	TextureGeometryMap::iterator tex_geometry_iter_end( new_geometries.end() );
	
	for( ; tex_geometry_iter != tex_geometry_iter_end; ++tex_geometry_iter ) {
		// Create buffer object.
		sg::BufferObject::Ptr buffer_object( new sg::BufferObject( sg::BufferObject::NORMALS | sg::BufferObject::TEX_COORDS ) );

		// Save buffer object, texture and geometry for delayed initialization.
		DelayedStep::Entry entry;

		entry.texture = tex_geometry_iter->first;
		entry.geometry = tex_geometry_iter->second;
		entry.buffer_object = buffer_object;

		delayed.entries.push_back( entry );
	}

	{
		boost::lock_guard<boost::mutex> lock( m_delayed_steps_mutex );
		m_delayed_steps.push_back( delayed );
	}

	queue_update();
}

std::shared_ptr<const flex::Model> PlanetDrawable::get_model( const flex::FlexID& id ) const {
	std::shared_ptr<const flex::Model> model = m_resource_manager.find_model( id );

	if( model == nullptr ) {
		// Try to load.
		if( !m_resource_manager.load_model( id ) ) {
#if !defined( NDEBUG )
			std::cerr << "Failed to load model: " << id.get() << std::endl;
#endif
			return model;
		}
#if !defined( NDEBUG )
		else {
			std::cout << "Model loaded: " << id.get() << std::endl;
		}
#endif

		model = m_resource_manager.find_model( id );
		assert( model );
	}

	return model;
}

std::shared_ptr<const sf::Texture> PlanetDrawable::get_texture( const flex::FlexID& id ) const {
	std::shared_ptr<const sf::Texture> texture = m_resource_manager.find_texture( id );

	if( texture == nullptr ) {
		// Try to load.
		bool result = m_resource_manager.prepare_texture( id );

		if( !result ) {
#if !defined( NDEBUG )
			std::cerr << "Failed to load texture: " << id.get() << std::endl;
#endif
			return texture;
		}
#if !defined( NDEBUG )
		else {
			std::cout << "Texture loaded: " << id.get() << std::endl;
		}
#endif

		texture = m_resource_manager.find_texture( id );
		assert( texture );
	}

	return texture;
}

void PlanetDrawable::handle_update() {
	{
		// Create pending delayed steps.
		boost::lock_guard<boost::mutex> lock( m_delayed_steps_mutex );

		for( std::size_t idx = 0; idx < m_delayed_steps.size(); ++idx ) {
			const DelayedStep& delayed = m_delayed_steps[idx];

			// Remove previous created steps of this chunk.
			m_steps[delayed.chunk_pos].clear();

			for( std::size_t entry_idx = 0; entry_idx < delayed.entries.size(); ++entry_idx ) {
				const DelayedStep::Entry& entry = delayed.entries[entry_idx];

				// Create the step.
				sg::RenderState r_state( get_render_state() );
				r_state.texture = entry.texture;

				// Load buffer object.
				entry.buffer_object->load( *entry.geometry );

				// Create step at renderer and store it (will overwrite old steps for same chunk).
				m_steps[delayed.chunk_pos].push_back( get_renderer().create_step( r_state, get_global_transform(), entry.buffer_object ) );
			}
		}

		// Done, clear delayed steps.
		m_delayed_steps.clear();
	}
}

void PlanetDrawable::handle_recalculate_global_transform() {
}

void PlanetDrawable::handle_update_render_state() {
	// When the render state is updated, we need to rebuild all steps. :-|
	sg::RenderState r_state = get_render_state();

	ChunkStepsMap::iterator cs_iter( m_steps.begin() );
	ChunkStepsMap::iterator cs_iter_end( m_steps.end() );
	
	for( ; cs_iter != cs_iter_end; ++cs_iter ) {
		for( std::size_t step_idx = 0; step_idx < cs_iter->second.size(); ++step_idx ) {
			// Preserve step proxy so the step won't be removed now (could trigger
			// garbage collectors).
			sg::StepProxy::Ptr step = cs_iter->second[step_idx];

			// Overwrite state's texture with step's texture.
			r_state.texture = step->get_step()->get_render_state().texture;

			// Reinitialize the step.
			cs_iter->second[step_idx] = get_renderer().create_step( r_state, get_global_transform(), step->get_step()->get_buffer_object() );
		}
	}
}
