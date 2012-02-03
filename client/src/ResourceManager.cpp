#include "ResourceManager.hpp"

#include <FlexWorld/ModelDriver.hpp>

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

	// Load image.
	sf::Image image;

	if( !image.LoadFromFile( m_base_path + id.as_path() ) ) {
		return false;
	}

	// Create and store texture.
	std::shared_ptr<sf::Texture> texture( new sf::Texture );

	if( !texture->LoadFromImage( image ) ) {
		return false;
	}

	{
		boost::lock_guard<boost::mutex> lock( m_textures_mutex );
		m_textures[id.get()] = texture;
	}

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
	std::shared_ptr<flex::Model> model( new flex::Model );

	try {
		*model = flex::ModelDriver::deserialize( buffer );
	}
	catch( const flex::ModelDriver::DeserializationException& /*e*/ ) {
		return false;
	}

	{
		boost::lock_guard<boost::mutex> lock( m_models_mutex );
		m_models[id.get()] = model;
	}

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
