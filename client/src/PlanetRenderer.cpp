#include <GL/glew.h>

#include "PlanetRenderer.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Model.hpp>
#include <FlexWorld/Class.hpp>

#include <SFML/System/Clock.hpp>
#include <cassert>
#include <iostream> // XXX 

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
			v0x + BOUNDARY_TOLERANCE >= rect.Left &&
			v0y + BOUNDARY_TOLERANCE >= rect.Top &&
			v0x - BOUNDARY_TOLERANCE <= rect.Left + rect.Width &&
			v0y - BOUNDARY_TOLERANCE <= rect.Top + rect.Height
		) &&
		(
			v1x + BOUNDARY_TOLERANCE >= rect.Left &&
			v1y + BOUNDARY_TOLERANCE >= rect.Top &&
			v1x - BOUNDARY_TOLERANCE <= rect.Left + rect.Width &&
			v1y - BOUNDARY_TOLERANCE <= rect.Top + rect.Height
		) &&
		(
			v2x + BOUNDARY_TOLERANCE >= rect.Left &&
			v2y + BOUNDARY_TOLERANCE >= rect.Top &&
			v2x - BOUNDARY_TOLERANCE <= rect.Left + rect.Width &&
			v2y - BOUNDARY_TOLERANCE <= rect.Top + rect.Height
		)
	;
}

PlanetRenderer::PlanetRenderer( const flex::Planet& planet, ResourceManager& resource_manager ) :
	m_camera( nullptr ),
	m_planet( planet ),
	m_resource_manager( resource_manager )
{
}

PlanetRenderer::~PlanetRenderer() {
}

void PlanetRenderer::prepare_chunk( const flex::Planet::Vector& chunk_pos ) {
	typedef std::map<std::shared_ptr<const sf::Texture>, std::shared_ptr<sg::BufferObject>> TextureVBOMap;

	sf::Clock clock;

	const flex::Chunk::Vector& chunk_size = m_planet.get_chunk_size();

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
	TextureVBOMap new_vbos;
	sg::Vertex vertex[3];
	const sf::FloatRect* coverage_rects[flex::NUM_FACES];

	flex::Chunk::Vector block_runner;
	for( block_runner.z = 0; block_runner.z < chunk_size.z; ++block_runner.z ) {
		for( block_runner.y = 0; block_runner.y < chunk_size.y; ++block_runner.y ) {
			for( block_runner.x = 0; block_runner.x < chunk_size.x; ++block_runner.x ) {
				// Skip empty blocks.
				block_cls = m_planet.find_block( chunk_pos, block_runner );

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
							block_runner.y + 1 < m_planet.get_chunk_size().y &&
							(nbor_block_cls = m_planet.find_block( chunk_pos, block_runner + flex::Chunk::Vector( 0, 1, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::DOWN_FACE )
						: 0
					;

					coverage_rects[flex::DOWN_FACE] =
						(
							block_runner.y > 0 &&
							(nbor_block_cls = m_planet.find_block( chunk_pos, block_runner - flex::Chunk::Vector( 0, 1, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::UP_FACE )
						: 0
					;

					coverage_rects[flex::LEFT_FACE] =
						(
							block_runner.x > 0 &&
							(nbor_block_cls = m_planet.find_block( chunk_pos, block_runner - flex::Chunk::Vector( 1, 0, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::RIGHT_FACE )
						: 0
					;

					coverage_rects[flex::RIGHT_FACE] =
						(
							block_runner.x + 1 < m_planet.get_chunk_size().x &&
							(nbor_block_cls = m_planet.find_block( chunk_pos, block_runner + flex::Chunk::Vector( 1, 0, 0 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::LEFT_FACE )
						: 0
					;

					coverage_rects[flex::FRONT_FACE] =
						(
							block_runner.z + 1 < m_planet.get_chunk_size().z &&
							(nbor_block_cls = m_planet.find_block( chunk_pos, block_runner + flex::Chunk::Vector( 0, 0, 1 ) )) &&
							(nbor_model = get_model( nbor_block_cls->get_model().get_id() ))
						)
						? &nbor_model->get_face_coverage( flex::BACK_FACE )
						: 0
					;

					coverage_rects[flex::BACK_FACE] =
						(
							block_runner.z > 0 &&
							(nbor_block_cls = m_planet.find_block( chunk_pos, block_runner - flex::Chunk::Vector( 0, 0, 1 ) )) &&
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

						// Get VBO or generate a new one.
						std::shared_ptr<sg::BufferObject>& vbo = new_vbos[texture];

						if( vbo == nullptr ) {
							vbo.reset( new sg::BufferObject( sg::BufferObject::TEX_COORDS | sg::BufferObject::NORMALS ) );
						}

						// Iterate over triangles.
						for( flex::Mesh::TriangleIndex tri_idx = 0; tri_idx < mesh.get_num_triangles(); ++tri_idx ) {
							const flex::Triangle& tri = mesh.get_triangle( tri_idx );

							// Get triangle's vertices.
							vertex[0] = mesh.get_vertex( tri.vertices[0] );
							vertex[1] = mesh.get_vertex( tri.vertices[1] );
							vertex[2] = mesh.get_vertex( tri.vertices[2] );

							// Apply block scale divisor.
							vertex[0].vector /= model->get_block_scale_divisor();
							vertex[1].vector /= model->get_block_scale_divisor();
							vertex[2].vector /= model->get_block_scale_divisor();

							/////// Hidden-face-removal.
							if(
								// Left face.
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

							vbo->add_vertex( vertex[0] );
							vbo->add_vertex( vertex[1] );
							vbo->add_vertex( vertex[2] );
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

	// Calculate absolute chunk position.
	uint32_t abs_chunk_pos = (
		chunk_pos.z * (m_planet.get_size().x * m_planet.get_size().y) +
		chunk_pos.y * (m_planet.get_size().x) +
		chunk_pos.x
	);

	// Pause rendering as we're modifying variables that are used by the render()
	// func.
	pause();

	//
	// Apply VBOs to main rendering pool.
	//

	// Remove all VBOs that were created for this chunk before.
	ChunkTextureMap::iterator chunk_tex_iter = m_chunk_textures.find( abs_chunk_pos );

	if( chunk_tex_iter != m_chunk_textures.end() ) {
		TextureVBOIndexMap::iterator vbo_iter = chunk_tex_iter->second.begin();
		TextureVBOIndexMap::iterator vbo_iter_end = chunk_tex_iter->second.end();

		for( ; vbo_iter != vbo_iter_end; ++vbo_iter ) {
			m_vbos[vbo_iter->first][vbo_iter->second].reset();
		}

		m_chunk_textures.erase( chunk_tex_iter );
	}
	
	// Add prepared VBOs to main pool.
	TextureVBOMap::iterator new_vbo_iter = new_vbos.begin();
	TextureVBOMap::iterator new_vbo_iter_end = new_vbos.end();
	std::size_t vbo_idx = 0;

	for( ; new_vbo_iter != new_vbo_iter_end; ++new_vbo_iter ) {
		// Get reference to VBO pool for a specific texture.
		VBOVector& vbos = m_vbos[new_vbo_iter->first];

		// Search for free slot.
		for( vbo_idx = 0; vbo_idx < vbos.size(); ++vbo_idx ) {
			if( vbos[vbo_idx] == nullptr ) {
				break;
			}
		}

		// Check if VBO pool needs to be enlarged.
		if( vbo_idx >= vbos.size() ) {
			vbos.resize( vbos.size() + 1 );
		}

		// Save VBO and create link between chunk/texture -> VBO.
		vbos[vbo_idx] = new_vbo_iter->second;
		m_chunk_textures[abs_chunk_pos][new_vbo_iter->first] = vbo_idx;
	}

	resume();

	//std::cout << clock.GetElapsedTime().AsMicroseconds() << " µs" << std::endl;
}

void PlanetRenderer::render() const {
	boost::lock_guard<boost::mutex> lock( m_render_mutex );

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glColor3f( 1, 1, 1 );

	// Apply camera.
	if( m_camera ) {
		glRotatef( m_camera->get_rotation().x, 1.0f, 0.0f, 0.0f );
		glRotatef( m_camera->get_rotation().y, 0.0f, 1.0f, 0.0f );
		glTranslatef( -m_camera->get_position().x, -m_camera->get_position().y, -m_camera->get_position().z );
	}

	glTranslatef( -50, -2.70f, -50 );

	std::size_t vbo_idx = 0;
	std::size_t num_vbos = 0;
	const VBOVector* vbos;
	std::shared_ptr<sg::BufferObject> buffer;

	TextureVBOVectorMap::const_iterator tex_vbo_iter( m_vbos.begin() );
	TextureVBOVectorMap::const_iterator tex_vbo_iter_end( m_vbos.end() );

	for( ; tex_vbo_iter != tex_vbo_iter_end; ++tex_vbo_iter ) {
		vbos = &tex_vbo_iter->second;
		num_vbos = vbos->size();

		// Bind texture.
		tex_vbo_iter->first->Bind();

		for( vbo_idx = 0; vbo_idx < num_vbos; ++vbo_idx ) {
			buffer = (*vbos)[vbo_idx];

			// Skip free VBO slots.
			if( buffer == nullptr ) {
				continue;
			}

			// Upload data to GPU if needed.
			if( buffer->is_upload_needed() ) {
				buffer->upload();
			}

			// Render VBO.
			buffer->render();
		}
	}
}

void PlanetRenderer::pause() const {
	m_render_mutex.lock();
}

void PlanetRenderer::resume() const {
	m_render_mutex.unlock();
}

void PlanetRenderer::set_camera( const Camera& camera ) {
	m_camera = &camera;
}

std::shared_ptr<const flex::Model> PlanetRenderer::get_model( const flex::FlexID& id ) const {
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

std::shared_ptr<const sf::Texture> PlanetRenderer::get_texture( const flex::FlexID& id ) const {
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
