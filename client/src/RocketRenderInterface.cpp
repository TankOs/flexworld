#include "RocketRenderInterface.hpp"

#include <FlexWorld/Config.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <cassert>

RocketRenderInterface::RocketRenderInterface( sf::RenderTarget& target ) :
	m_render_target( target )
{
}

void RocketRenderInterface::RenderGeometry(
	Rocket::Core::Vertex* vertices,
	int /*num_vertices*/,
	int* indices,
	int num_indices,
	Rocket::Core::TextureHandle texture_handle,
	const Rocket::Core::Vector2f& translation
) {
	// Get texture.
	const sf::Texture* texture = reinterpret_cast<const sf::Texture*>( texture_handle );

	// Unfortunately we can't directly send vertex data to SFML as it doesn't
	// work with indices. Therefore prepare vertex array.
	sf::VertexArray vertex_array( sf::Triangles, num_indices );

	for( int index_idx = 0; index_idx < num_indices; ++index_idx ) {
		const Rocket::Core::Vertex& src_vertex = vertices[indices[index_idx]];
		sf::Vertex& tgt_vertex = vertex_array[index_idx];

		tgt_vertex.position.x = src_vertex.position.x;
		tgt_vertex.position.y = src_vertex.position.y;

		tgt_vertex.color.r = src_vertex.colour.red;
		tgt_vertex.color.g = src_vertex.colour.green;
		tgt_vertex.color.b = src_vertex.colour.blue;
		tgt_vertex.color.a = src_vertex.colour.alpha;

		if( texture ) {
			tgt_vertex.texCoords.x = src_vertex.tex_coord.x * static_cast<float>( texture->getSize().x );
			tgt_vertex.texCoords.y = src_vertex.tex_coord.y * static_cast<float>( texture->getSize().y );
		}
	}

	// Render.
	sf::Transform transform;
	transform.translate( translation.x, translation.y );

	m_render_target.draw(
		vertex_array,
		sf::RenderStates(
			sf::BlendAlpha,
			transform,
			texture,
			nullptr
		)
	);
}

Rocket::Core::CompiledGeometryHandle RocketRenderInterface::CompileGeometry(
	Rocket::Core::Vertex* vertices,
	int /*num_vertices*/,
	int* indices,
	int num_indices,
	Rocket::Core::TextureHandle texture_handle
) {
	GeometryInfo* info = new GeometryInfo;

	// Save texture if any.
	const sf::Texture* texture = reinterpret_cast<const sf::Texture*>( texture_handle );

	if( texture ) {
		info->texture = texture;
	}

	// Setup vertex array.
	info->vertex_array.reset( new sf::VertexArray( sf::Triangles, num_indices ) );

	for( int index_idx = 0; index_idx < num_indices; ++index_idx ) {
		const Rocket::Core::Vertex& src_vertex = vertices[indices[index_idx]];
		sf::Vertex& tgt_vertex = (*info->vertex_array)[index_idx];

		tgt_vertex.position.x = src_vertex.position.x;
		tgt_vertex.position.y = src_vertex.position.y;

		tgt_vertex.color.r = src_vertex.colour.red;
		tgt_vertex.color.g = src_vertex.colour.green;
		tgt_vertex.color.b = src_vertex.colour.blue;
		tgt_vertex.color.a = src_vertex.colour.alpha;

		if( info->texture ) {
			tgt_vertex.texCoords.x = src_vertex.tex_coord.x * static_cast<float>( info->texture->getSize().x );
			tgt_vertex.texCoords.y = src_vertex.tex_coord.y * static_cast<float>( info->texture->getSize().y );
		}
	}

	return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>( info );
}

void RocketRenderInterface::RenderCompiledGeometry(
	Rocket::Core::CompiledGeometryHandle geometry,
	const Rocket::Core::Vector2f& translation
) {
	GeometryInfo* info = reinterpret_cast<GeometryInfo*>( geometry );
	assert( info );

	sf::Transform transform;
	transform.translate( translation.x, translation.y );

	m_render_target.draw(
		*info->vertex_array,
		sf::RenderStates(
			sf::BlendAlpha,
			transform,
			info->texture,
			nullptr
		)
	);
}

void RocketRenderInterface::ReleaseCompiledGeometry(
	Rocket::Core::CompiledGeometryHandle geometry
) {
	GeometryInfo* info = reinterpret_cast<GeometryInfo*>( geometry );
	assert( info );

	delete info;
}

bool RocketRenderInterface::LoadTexture(
	Rocket::Core::TextureHandle& texture_handle,
	Rocket::Core::Vector2i& texture_dimensions,
	const Rocket::Core::String& source
) {
	// Try to load the texture.
	sf::Texture* texture = new sf::Texture;

	if( !texture->loadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( source.CString() ) ) ) {
		delete texture;
		return false;
	}

	texture->setSmooth( true );

	texture_dimensions.x = texture->getSize().x;
	texture_dimensions.y = texture->getSize().y;
	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>( texture );

	return true;
}

bool RocketRenderInterface::GenerateTexture(
	Rocket::Core::TextureHandle& texture_handle,
	const Rocket::Core::byte* source,
	const Rocket::Core::Vector2i& source_dimensions
) {
	sf::Texture* texture = new sf::Texture;

	if( !texture->create( source_dimensions.x, source_dimensions.y ) ) {
		std::cerr << "Failed to generate texture." << std::endl;
		delete texture;
		return false;
	}

	texture->update( source );

	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>( texture );

	return true;
}

void RocketRenderInterface::ReleaseTexture( Rocket::Core::TextureHandle texture_handle ) {
	delete reinterpret_cast<sf::Texture*>( texture_handle );
}

void RocketRenderInterface::EnableScissorRegion( bool enable ) {
	if( enable ) {
		glEnable( GL_SCISSOR_TEST );
	}
	else {
		glDisable( GL_SCISSOR_TEST );
	}
}

void RocketRenderInterface::SetScissorRegion( int x, int y, int width, int height ) {
	glScissor( x, y, width, height );
}

RocketRenderInterface::GeometryInfo::GeometryInfo() :
	texture( nullptr )
{
}
