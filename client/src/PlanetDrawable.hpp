#pragma once

#include <FlexWorld/Planet.hpp>

#include <FWSG/Drawable.hpp>
#include <FWSG/StepProxy.hpp>
#include <boost/thread.hpp>

class ResourceManager;

namespace fw {
class Planet;
class FlexID;
class Model;
}

namespace sf {
class Texture;
}

/** FWSG drawable for rendering planets.
 */
class PlanetDrawable : public sg::Drawable {
	public:
		typedef std::shared_ptr<PlanetDrawable> Ptr; ///< Shared pointer.
		typedef std::shared_ptr<const PlanetDrawable> PtrConst; ///< Shared pointer to const.

		~PlanetDrawable();

		/** Create.
		 * @param planet Planet to render.
		 * @param resource_manager Resource manager.
		 * @param renderer Renderer.
		 * @return PlanetDrawable.
		 */
		static Ptr create( const fw::Planet& planet, ResourceManager& resource_manager, sg::Renderer& renderer );

		/** Set view radius.
		 * @param radius Radius (in chunks).
		 */
		void set_view_radius( fw::Planet::ScalarType radius );

		/** Prepare chunk for rendering.
		 * @param chunk_pos Chunk position.
		 */
		void prepare_chunk( const fw::Planet::Vector& chunk_pos );

		/** Cancel preparing chunks.
		 */
		void cancel_chunk_prepare();

	private:
		struct DelayedStep {
			/** --
			 */
			struct Entry {
				std::shared_ptr<const sf::Texture> texture; ///< Texture.
				std::shared_ptr<const sg::Geometry> geometry; ///< Geometry.
				std::shared_ptr<sg::BufferObject> buffer_object; ///< Buffer object.
			};

			std::vector<Entry> entries;
			uint32_t chunk_pos;
		};

		void handle_update();
		void handle_recalculate_global_transform();
		void handle_update_render_state();

		typedef uint32_t ChunkPosition;
		typedef std::vector<sg::StepProxy::Ptr> StepVector;
		typedef std::map<ChunkPosition, StepVector> ChunkStepsMap;
		typedef std::vector<DelayedStep> DelayedStepVector;

		PlanetDrawable( const fw::Planet& planet, ResourceManager& resource_manager, sg::Renderer& renderer );
		std::shared_ptr<const fw::Model> get_model( const fw::FlexID& id ) const;
		std::shared_ptr<const sf::Texture> get_texture( const fw::FlexID& id ) const;

		ChunkStepsMap m_steps;

		const fw::Planet* m_planet;
		ResourceManager& m_resource_manager;

		fw::Planet::ScalarType m_view_radius;

		boost::mutex m_delayed_steps_mutex;
		DelayedStepVector m_delayed_steps;
};
