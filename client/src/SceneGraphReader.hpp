#pragma once

#include <FlexWorld/Types.hpp>

#include <FWMS/Reader.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <list>

class PlanetDrawable;
class EntityGroupNode;
class ResourceManager;
class SessionState;

namespace fw {
class World;
class LockFacility;
}

namespace sg {
class Node;
class Renderer;
}

/** Message system reader for scene graph actions.
 *
 * The reader runs a separate thread for preparing chunk geometry in the planet
 * renderer. It also adds and removes entities from the scene graph.
 */
class SceneGraphReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		SceneGraphReader();

		/** Dtor.
		 */
		~SceneGraphReader();

		/** Set scene graph root node.
		 * @param root_node Root node.
		 */
		void set_root_node( std::shared_ptr<sg::Node> root_node );

		/** Set world.
		 * @param world World.
		 */
		void set_world( const fw::World& world );

		/** Set resource manager.
		 * @param resource_manager Resource manager.
		 */
		void set_resource_manager( ResourceManager& resource_manager );

		/** Set FWSG renderer.
		 * @param renderer Renderer.
		 */
		void set_renderer( sg::Renderer& renderer );

		/** Set session state.
		 * @param session_state Session state.
		 */
		void set_session_state( const SessionState& session_state );

		/** Set lock facility.
		 * @param lock_facility Lock facility.
		 */
		void set_lock_facility( fw::LockFacility& lock_facility );

	private:
		typedef std::list<fw::ChunkVector> ChunkPositionList;
		typedef std::list<fw::EntityID> EntityList;

		void handle_message( const ms::Message& message );

		void reset_nodes();
		void add_entity( const ms::Message& message );
		void prepare_loop();

		boost::thread m_prepare_thread;
		boost::mutex m_prepare_data_mutex;
		boost::mutex m_prepare_condition_mutex;
		boost::mutex m_scene_graph_mutex;
		boost::condition_variable m_prepare_condition;

		ChunkPositionList m_chunk_positions;
		EntityList m_entities;

		std::shared_ptr<sg::Node> m_root_node;
		std::shared_ptr<PlanetDrawable> m_planet_drawable;
		std::shared_ptr<EntityGroupNode> m_entity_group_node;

		const fw::World* m_world;
		ResourceManager* m_resource_manager;
		sg::Renderer* m_renderer;
		const SessionState* m_session_state;
		fw::LockFacility* m_lock_facility;

		bool m_terminate_prepare_thread;
};
