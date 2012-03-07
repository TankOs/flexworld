#pragma once

#include <FlexWorld/Planet.hpp>

#include <FWSG/Drawable.hpp>
#include <FWSG/StepProxy.hpp>
#include <boost/thread.hpp>

class ResourceManager;

namespace flex {
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
		static Ptr create( const flex::Planet& planet, ResourceManager& resource_manager, sg::Renderer& renderer );

		/** Set view radius.
		 * @param radius Radius (in chunks).
		 */
		void set_view_radius( flex::Planet::ScalarType radius );

		/** Prepare chunk for rendering.
		 * @param chunk_pos Chunk position.
		 */
		void prepare_chunk( const flex::Planet::Vector& chunk_pos );

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

		PlanetDrawable( const flex::Planet& planet, ResourceManager& resource_manager, sg::Renderer& renderer );
		std::shared_ptr<const flex::Model> get_model( const flex::FlexID& id ) const;
		std::shared_ptr<const sf::Texture> get_texture( const flex::FlexID& id ) const;

		ChunkStepsMap m_steps;

		const flex::Planet* m_planet;
		ResourceManager& m_resource_manager;

		flex::Planet::ScalarType m_view_radius;

		boost::mutex m_delayed_steps_mutex;
		DelayedStepVector m_delayed_steps;
};
