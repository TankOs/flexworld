#pragma once

#include <Rocket/Core.h>
#include <memory>

namespace sf {
class Texture;
class RenderTarget;
class VertexArray;
}

/** Render interface for libRocket using SFML's rendering stuff.
 */
class RocketRenderInterface : public Rocket::Core::RenderInterface {
	public:
		/** Ctor.
		 * @param render_target SFML render target.
		 */
		RocketRenderInterface( sf::RenderTarget& target );

		/** Render geometry without optimizations.
		 * @param vertices Vertices.
		 * @param num_vertices Number of vertices.
		 * @param indices Indices.
		 * @param num_indices Number of indices.
		 * @param texture_handle Texture handle.
		 * @param translation Translation.
		 */
		void RenderGeometry(
			Rocket::Core::Vertex* vertices,
			int num_vertices,
			int* indices,
			int num_indices,
			Rocket::Core::TextureHandle texture_handle,
			const Rocket::Core::Vector2f& translation
		);

		/** Compile geometry.
		 * @param vertices Vertices.
		 * @param num_vertices Number of vertices.
		 * @param indices Indices.
		 * @param num_indices Number of indices.
		 * @param texture Texture handle.
		 * @return Geometry handle.
		 */
		Rocket::Core::CompiledGeometryHandle CompileGeometry(
			Rocket::Core::Vertex* vertices,
			int num_vertices,
			int* indices,
			int num_indices,
			Rocket::Core::TextureHandle texture_handle
		);

		/** Render compiled geometry.
		 * @param geometry Geometry handle.
		 * @param translation Translation.
		 */
		void RenderCompiledGeometry(
			Rocket::Core::CompiledGeometryHandle geometry,
			const Rocket::Core::Vector2f& translation
		);

		/** Released compiled geometry.
		 * @param geometry Geometry handle.
		 */
		void ReleaseCompiledGeometry(
			Rocket::Core::CompiledGeometryHandle geometry
		);

		/** Load texture.
		 * @param texture_handle Output texture handle.
		 * @param texture_dimensions Output texture dimensions.
		 * @param source Source filename/ID.
		 * @return true on success.
		 */
		bool LoadTexture(
			Rocket::Core::TextureHandle& texture_handle,
			Rocket::Core::Vector2i& texture_dimensions,
			const Rocket::Core::String& source
		);

		/** Generate texture.
		 * @param texture_handle Output texture handle.
		 * @param source Image data (RGBA, 1 byte per element).
		 * @param source_dimensions Dimensions.
		 * @return true on success.
		 */
		bool GenerateTexture(
			Rocket::Core::TextureHandle& texture_handle,
			const Rocket::Core::byte* source,
			const Rocket::Core::Vector2i& source_dimensions
		);

		/** Release texture.
		 * @param texture_handle Texture handle.
		 */
		void ReleaseTexture( Rocket::Core::TextureHandle texture_handle );

		/** Enable or disable scissor test.
		 * @param enable true to enable.
		 */
		void EnableScissorRegion( bool enable );

		/** Set scissor test region.
		 * @param x X.
		 * @param y Y.
		 * @param width Width.
		 * @param height Height.
		 */
		void SetScissorRegion( int x, int y, int width, int height );

	private:
		struct GeometryInfo {
			GeometryInfo();

			std::unique_ptr<sf::VertexArray> vertex_array;
			const sf::Texture* texture;
		};

		sf::RenderTarget& m_render_target;
};
