#include <GL/glew.h>

#include "ResourceManager.hpp"

#include <FlexWorld/ModelDriver.hpp>

#include <FWSG/BufferObject.hpp>
#include <fstream>
#include <iterator>
#include <cassert>

ResourceManager::ResourceManager() :
	m_anisotropy_level( 0 ),
	m_texture_filter( TRILINEAR_FILTER )
{
}

void ResourceManager::set_base_path( const std::string& path ) {
	m_base_path = path;

	// Append trailing slash.
	if( m_base_path.size() > 0 && m_base_path[m_base_path.size() - 1] != '/' ) {
		m_base_path += '/';
	}
}

bool ResourceManager::find_path( const fw::FlexID& id, std::string& path ) const {
	assert( id.is_valid_resource() );

	// Build path and try to open.
	std::string check_path = m_base_path + id.as_path();
	std::ifstream in_file( check_path.c_str(), std::ios::in | std::ios::binary );

	if( !in_file.good() ) {
		return false;
	}

	path = check_path;
	return true;
}

bool ResourceManager::load_texture( const fw::FlexID& id ) {
	// Erase texture with same ID.
	if( find_texture( id ) ) {
		boost::lock_guard<boost::mutex> lock( m_textures_mutex );
		m_textures.erase( id.get() );
	}

	// Load image.
	sf::Image image;

	if( !image.loadFromFile( m_base_path + id.as_path() ) ) {
#if !defined( NDEBUG )
		std::cerr << "ERROR: Failed to load image " << id.get() << "." << std::endl;
#endif
		return false;
	}

	// Create and setup texture.
	TexturePtr texture( new sf::Texture );

	texture->loadFromImage( image );
	setup_texture( texture, &image );

	{
		boost::lock_guard<boost::mutex> lock( m_textures_mutex );
		m_textures[id.get()] = texture;
	}

#if !defined( NDEBUG )
	std::cout << "Loaded: " << id.get() << std::endl;
#endif

	return true;
}

std::shared_ptr<const sf::Texture> ResourceManager::find_texture( const fw::FlexID& id ) const {
	std::shared_ptr<const sf::Texture> texture;

	// Guarded.
	{
		boost::lock_guard<boost::mutex> lock( m_textures_mutex );

		TextureMap::const_iterator tex_iter( m_textures.find( id.get() ) );

		if( tex_iter != m_textures.end() ) {
			texture = tex_iter->second;
		}
	}

	return texture;
}

void ResourceManager::garbage_collect() {
	boost::lock_guard<boost::mutex> textures_lock( m_textures_mutex );
	boost::lock_guard<boost::mutex> models_lock( m_models_mutex );

	// Collect textures.
	{
		std::vector<std::string> ids_to_kill;

		TextureMap::iterator tex_iter( m_textures.begin() );
		TextureMap::iterator tex_iter_end( m_textures.end() );
		
		for( ; tex_iter != tex_iter_end; ++tex_iter ) {
			if( tex_iter->second.use_count() <= 1 ) {
				ids_to_kill.push_back( tex_iter->first );
			}
		}

		for( std::size_t kill_idx = 0; kill_idx < ids_to_kill.size(); ++kill_idx ) {
			m_textures.erase( ids_to_kill[kill_idx] );
		}
	}
}

bool ResourceManager::load_model( const fw::FlexID& id ) {
	// Erase model with same ID.
	if( find_model( id ) ) {
		boost::lock_guard<boost::mutex> lock( m_models_mutex );
		m_models.erase( id.get() );
	}

	// Open file.
	std::string path = m_base_path + id.as_path();
	std::ifstream in_file( path.c_str(), std::ios::in | std::ios::binary );

	if( !in_file.is_open() ) {
#if !defined( NDEBUG )
		std::cerr << "ERROR: Failed to read model " << id.get() << "." << std::endl;
#endif
		return false;
	}

	// Read whole file.
	fw::ModelDriver::Buffer buffer;
	in_file >> std::noskipws;

	std::copy(
		std::istream_iterator<char>( in_file ),
		std::istream_iterator<char>(),
		std::back_inserter( buffer )
	);

	in_file.close();

	// Deserialize model.
	ModelPtr model( new fw::Model );

	try {
		*model = fw::ModelDriver::deserialize( buffer );
	}
	catch( const fw::ModelDriver::DeserializationException& e ) {
#if !defined( NDEBUG )
		std::cerr << "Exception when loading a model: " << e.what() << std::endl;
#endif
		return false;
	}

	{
		boost::lock_guard<boost::mutex> lock( m_models_mutex );
		m_models[id.get()] = model;
	}

#if !defined( NDEBUG )
	std::cout << "Loaded: " << id.get() << std::endl;
#endif

	return true;
}

std::shared_ptr<const fw::Model> ResourceManager::find_model( const fw::FlexID& id ) const {
	std::shared_ptr<const fw::Model> model;

	{
		boost::lock_guard<boost::mutex> lock( m_models_mutex );

		ModelMap::const_iterator model_iter( m_models.find( id.get() ) );

		if( model_iter != m_models.end() ) {
			model = model_iter->second;
		}
	}

	return model;
}

bool ResourceManager::prepare_texture( const fw::FlexID& id ) {
	boost::lock_guard<boost::mutex> lock( m_textures_mutex );

	// Remove texture with same ID, both prepared and finalized.
	m_prepared_textures.erase( id.get() );
	m_textures.erase( id.get() );

	// Create new image and try to load.
	ImagePtr image( new sf::Image );

	if( !image->loadFromFile( m_base_path + id.as_path() ) ) {
		return false;
	}

	// Create new texture.
	TexturePtr texture( new sf::Texture );

	// Insert the prepared texture.
	m_textures[id.get()] = texture;
	m_prepared_textures[id.get()] = image;

#if !defined( NDEBUG )
	std::cout << "Loaded: " << id.get() << "." << std::endl;
#endif

	return true;
}

void ResourceManager::finalize_prepared_textures() {
	boost::lock_guard<boost::mutex> lock( m_textures_mutex );

	if( !m_prepared_textures.size() ) {
		return;
	}

	PreparedTextureMap::iterator tex_iter( m_prepared_textures.begin() );
	PreparedTextureMap::iterator tex_iter_end( m_prepared_textures.end() );
	
	for( ; tex_iter != tex_iter_end; ++tex_iter ) {
		m_textures[tex_iter->first]->loadFromImage( *tex_iter->second );

		// Setup texture.
		setup_texture( m_textures[tex_iter->first], &*tex_iter->second );
	}

	m_prepared_textures.clear();
}

bool ResourceManager::prepare_buffer_object_group( const fw::FlexID& model_id ) {
	assert( model_id.is_valid_resource() );
	assert( find_buffer_object_group( model_id ) == nullptr );

	boost::lock_guard<boost::mutex> lock( m_buffer_object_groups_mutex );

	// Get model.
	std::shared_ptr<const fw::Model> model = find_model( model_id );

	// If not found, try to load it.
	if( model == nullptr ) {
		if( !load_model( model_id ) ) {
			return false;
		}

		model = find_model( model_id );
		assert( model != nullptr );
	}

	// Create the buffer object group and associate it.
	BufferObjectGroup::Ptr group(
		new BufferObjectGroup(
			model->get_num_meshes(),
			sg::BufferObject::NORMALS | sg::BufferObject::TEX_COORDS
		)
	);

	m_buffer_object_groups[model_id.get()] = group;
	m_prepared_buffer_object_groups[model_id.get()] = model;

	return true;
}

BufferObjectGroup::PtrConst ResourceManager::find_buffer_object_group( const fw::FlexID& model_id ) const {
	assert( model_id.is_valid_resource() );
	boost::lock_guard<boost::mutex> lock( m_buffer_object_groups_mutex );

	BufferObjectGroupMap::const_iterator iter = m_buffer_object_groups.find( model_id.get() );

	return iter == m_buffer_object_groups.end() ? nullptr : iter->second;
}

void ResourceManager::finalize_prepared_buffer_object_groups() {
	boost::lock_guard<boost::mutex> lock( m_buffer_object_groups_mutex );

	PreparedBufferObjectGroupMap::iterator bo_iter( m_prepared_buffer_object_groups.begin() );
	PreparedBufferObjectGroupMap::iterator bo_iter_end( m_prepared_buffer_object_groups.end() );
	
	for( ; bo_iter != bo_iter_end; ++bo_iter ) {
		ModelPtrConst model = bo_iter->second;
		BufferObjectGroup::Ptr group = m_buffer_object_groups[bo_iter->first];

		for( std::size_t mesh_idx = 0; mesh_idx < model->get_num_meshes(); ++mesh_idx ) {
			group->get_buffer_object( mesh_idx )->load( model->get_mesh( mesh_idx ).get_geometry() );
		}
	}
}

void ResourceManager::set_anisotropy_level( uint8_t level ) {
	float max_level = 0.0f;

	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_level );
	m_anisotropy_level = std::min( max_level, static_cast<float>( level ) );

	// Resetup all textures.
	setup_all_textures();
}

void ResourceManager::setup_texture( TexturePtr texture, const sf::Image* image ) const {
	sf::Texture::bind( &*texture );

	// Texture filter.
	if( m_texture_filter == TRILINEAR_FILTER ) {
		sf::Image image_copy;

		// No source image available, copy from texture.
		if( !image ) {
			image_copy = texture->copyToImage();
			image = &image_copy;
		}

		gluBuild2DMipmaps(
			GL_TEXTURE_2D,
			GL_RGBA, 
			image->getSize().x,
			image->getSize().y,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image->getPixelsPtr()
		);

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else if( m_texture_filter == BILINEAR_FILTER ) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else if( m_texture_filter == NEAREST_FILTER ) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	// Anisotropic filter.
	if( m_anisotropy_level > 0 ) {
		glTexParameterf(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAX_ANISOTROPY_EXT,
			std::pow( 2.0f, static_cast<float>( m_anisotropy_level - 1 ) )
		);
	}

	glBindTexture( GL_TEXTURE_2D, 0 );
}

void ResourceManager::setup_all_textures() const {
	TextureMap::const_iterator tex_iter( m_textures.begin() );
	TextureMap::const_iterator tex_iter_end( m_textures.end() );
	
	for( ; tex_iter != tex_iter_end; ++tex_iter ) {
		setup_texture( tex_iter->second, nullptr );
	}
}

void ResourceManager::set_texture_filter( TextureFilter filter ) {
	m_texture_filter = filter;

	setup_all_textures();
}
