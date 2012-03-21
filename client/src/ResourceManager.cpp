#include "ResourceManager.hpp"

#include <FlexWorld/ModelDriver.hpp>

#include <FWSG/BufferObject.hpp>
#include <fstream>
#include <iterator>

void ResourceManager::set_base_path( const std::string& path ) {
	m_base_path = path;

	// Append trailing slash.
	if( m_base_path.size() > 0 && m_base_path[m_base_path.size() - 1] != '/' ) {
		m_base_path += '/';
	}
}

bool ResourceManager::load_texture( const flex::FlexID& id ) {
	// Erase texture with same ID.
	if( find_texture( id ) ) {
		boost::lock_guard<boost::mutex> lock( m_textures_mutex );
		m_textures.erase( id.get() );
	}

	// Create texture and load image into it.
	TexturePtr texture( new sf::Texture );

	if( !texture->loadFromFile( m_base_path + id.as_path() ) ) {
#if !defined( NDEBUG )
		std::cerr << "ERROR: Failed to load texture " << id.get() << "." << std::endl;
#endif
		return false;
	}

	texture->setSmooth( true );

	{
		boost::lock_guard<boost::mutex> lock( m_textures_mutex );
		m_textures[id.get()] = texture;
	}

#if !defined( NDEBUG )
	std::cout << "Texture loaded: " << id.get() << std::endl;
#endif

	return true;
}

std::shared_ptr<const sf::Texture> ResourceManager::find_texture( const flex::FlexID& id ) const {
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

bool ResourceManager::load_model( const flex::FlexID& id ) {
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
	flex::ModelDriver::Buffer buffer;
	in_file >> std::noskipws;

	std::copy(
		std::istream_iterator<char>( in_file ),
		std::istream_iterator<char>(),
		std::back_inserter( buffer )
	);

	in_file.close();

	// Deserialize model.
	ModelPtr model( new flex::Model );

	try {
		*model = flex::ModelDriver::deserialize( buffer );
	}
	catch( const flex::ModelDriver::DeserializationException& e ) {
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
	std::cout << "Model loaded: " << id.get() << std::endl;
#endif

	return true;
}

std::shared_ptr<const flex::Model> ResourceManager::find_model( const flex::FlexID& id ) const {
	std::shared_ptr<const flex::Model> model;

	{
		boost::lock_guard<boost::mutex> lock( m_models_mutex );

		ModelMap::const_iterator model_iter( m_models.find( id.get() ) );

		if( model_iter != m_models.end() ) {
			model = model_iter->second;
		}
	}

	return model;
}

bool ResourceManager::prepare_texture( const flex::FlexID& id ) {
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
	std::cout << "Loaded texture " << id.get() << "." << std::endl;
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
		m_textures[tex_iter->first]->setSmooth( true );
	}

	m_prepared_textures.clear();
}

bool ResourceManager::prepare_buffer_object_group( const flex::FlexID& model_id ) {
	assert( model_id.is_valid_resource() );
	assert( find_buffer_object_group( model_id ) == nullptr );

	boost::lock_guard<boost::mutex> lock( m_buffer_object_groups_mutex );

	// Get model.
	std::shared_ptr<const flex::Model> model = find_model( model_id );

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

BufferObjectGroup::PtrConst ResourceManager::find_buffer_object_group( const flex::FlexID& model_id ) const {
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
