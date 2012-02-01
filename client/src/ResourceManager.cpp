#include "ResourceManager.hpp"

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

	m_textures[id.get()] = texture;
	return true;
}

std::shared_ptr<const sf::Texture> ResourceManager::find_texture( const flex::FlexID& id ) const {
	TextureMap::const_iterator tex_iter( m_textures.find( id.get() ) );

	if( tex_iter == m_textures.end() ) {
		return std::shared_ptr<const sf::Texture>();
	}

	return tex_iter->second;
}

void ResourceManager::garbage_collect() {
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
