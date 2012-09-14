#include "SceneGraphReader.hpp"
#include "PlanetDrawable.hpp"
#include "EntityGroupNode.hpp"
#include "SessionState.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <cassert>

static const ms::HashValue BEAM_ID = ms::string_hash( "beam" );
static const ms::HashValue CHUNK_UPDATE_ID = ms::string_hash( "chunk_update" );
static const ms::HashValue CLASS_ID = ms::string_hash( "class" );
static const ms::HashValue CREATE_ENTITY_ID = ms::string_hash( "create_entity" );
static const ms::HashValue HEADING_ID = ms::string_hash( "heading" );
static const ms::HashValue HOOK_ID = ms::string_hash( "hook" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue PARENT_ID_ID = ms::string_hash( "parent_id" );
static const ms::HashValue PLANET_ID_ID = ms::string_hash( "planet_id" );
static const ms::HashValue POSITION_ID = ms::string_hash( "position" );

SceneGraphReader::SceneGraphReader() :
	ms::Reader(),
	m_world( nullptr ),
	m_resource_manager( nullptr ),
	m_renderer( nullptr ),
	m_session_state( nullptr ),
	m_lock_facility( nullptr ),
	m_terminate_prepare_thread( false )
{
	// Launch thread.
	m_prepare_thread = boost::thread( &SceneGraphReader::prepare_loop, this );
}

SceneGraphReader::~SceneGraphReader() {
	// Stop prepare thread.
	{
		boost::lock_guard<boost::mutex> lock( m_prepare_data_mutex );

		m_terminate_prepare_thread = true;
		m_prepare_condition.notify_one();
	}

	m_prepare_thread.join();
}

void SceneGraphReader::set_root_node( std::shared_ptr<sg::Node> root_node ) {
	m_root_node = root_node;
}

void SceneGraphReader::set_resource_manager( ResourceManager& resource_manager ) {
	m_resource_manager = &resource_manager;
}

void SceneGraphReader::set_renderer( sg::Renderer& renderer ) {
	m_renderer = &renderer;
}

void SceneGraphReader::set_session_state( const SessionState& session_state ) {
	m_session_state = &session_state;
}

void SceneGraphReader::set_world( const fw::World& world ) {
	m_world = &world;
}

void SceneGraphReader::set_lock_facility( fw::LockFacility& lock_facility ) {
	m_lock_facility = &lock_facility;
}

void SceneGraphReader::handle_message( const ms::Message& message ) {
	assert( m_root_node );
	assert( m_world );
	assert( m_resource_manager );
	assert( m_renderer );
	assert( m_session_state );
	assert( m_lock_facility );

	if( message.get_id() == BEAM_ID ) {
		std::cout << "SceneGraphReader: Beam received. Clearing scene graph." << std::endl;

		reset_nodes();
	}
	else if( message.get_id() == CHUNK_UPDATE_ID ) {
		const fw::ChunkVector* chunk_position = message.find_property<fw::ChunkVector>( POSITION_ID );

		if( chunk_position ) {
			boost::lock_guard<boost::mutex> data_lock( m_prepare_data_mutex );
			m_chunk_positions.push_back( *chunk_position );

			m_prepare_condition.notify_one();
		}
	}
	else if( message.get_id() == CREATE_ENTITY_ID ) {
		add_entity( message );
	}
}

void SceneGraphReader::add_entity( const ms::Message& message ) {
	// Do nothing if own entity hasn't been received yet.
	if( m_session_state->own_entity_received == false ) {
		std::cout << "WARNING: SceneGraphReader: New entity can't be added to scene graph because own entity wasn't detected yet." << std::endl;
	}

	const fw::EntityID* entity_id = message.find_property<fw::EntityID>( ID_ID );
	const fw::PlanetVector* position = message.find_property<fw::PlanetVector>( POSITION_ID );
	const float* heading = message.find_property<float>( HEADING_ID );

	assert( entity_id != nullptr );
	assert( position != nullptr );

	bool skip = false;

	// Skip if own entity.
	if( *entity_id == m_session_state->own_entity_id ) {
		std::cout << "SceneGraphReader: Received own entity." << std::endl;
		skip = true;
	}
	else {
		// Check if any parent is the player entity or if any parent is attached
		// to an invisible hook.
		m_lock_facility->lock_world( true );

		const fw::Entity* entity = m_world->find_entity( *entity_id );
		assert( entity != nullptr );

		const fw::Entity* child = entity;
		const fw::Entity* parent = child->get_parent();

		while( parent != nullptr && skip == false ) {
			if( parent->get_id() == *entity_id ) {
				skip = true;
				continue;
			}

			if( *parent->get_class().find_hook( parent->get_child_hook( *child ) ) == fw::Class::INVISIBLE_HOOK ) {
				skip = true;
				continue;
			}

			child = parent;
			parent = child->get_parent();
		}

		m_lock_facility->lock_world( false );
	}

	// If not skipped, add to scene graph.
	if( skip == false ) {
		boost::lock_guard<boost::mutex> lock( m_prepare_data_mutex );

		m_entities.push_back( *entity_id );
		m_prepare_condition.notify_one();

		std::cout << "SceneGraphReader: Added entity to scene graph." << std::endl;
	}
	else {
		std::cout << "SceneGraphReader: Entity not added to scene graph." << std::endl;
	}

}
	/*
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

#if !defined( NDEBUG )
	std::cout
		<< "Received entity #" << msg.get_id() << " (" << msg.get_class() << ") @ "
		<< msg.get_position().x << ", " << msg.get_position().y << ", " << msg.get_position().z
		<< " (" << msg.get_heading() << "°)"
	;
	
	if( msg.has_parent() ) {
		std::cout << " Parent entity #" << msg.get_parent_id() << ", hook " << msg.get_parent_hook();
	}
	else {
		std::cout << " No parent entity.";
	}

	std::cout << std::endl;
#endif

	// If own entity set position.
	if( msg.get_id() == m_session_state->own_entity_id ) {
		m_session_state->own_entity_received = true;

		m_camera.set_position( msg.get_position() );
		m_camera.set_rotation( sf::Vector3f( 0, msg.get_heading(), 0 ) );

		m_update_eyepoint = true;
	}

	// Add to entity drawable. Skip if
	// - entity is attached to invisible hook
	// - entity is attached to ourself
	bool skip = false;

	if( m_entity_group_node == nullptr || msg.get_id() == m_session_state->own_entity_id ) {
		skip = true;
	}
	else if( msg.has_parent() ) {
		get_shared().lock_facility->lock_world( true );

		// Get entity and parent entity.
		const fw::Entity* ent = get_shared().world->find_entity( msg.get_id() );
		assert( ent != nullptr );

		const fw::Entity* parent_ent = ent->get_parent();
		assert( parent_ent != nullptr );

		if( parent_ent == nullptr ) {
			std::cerr
				<< "*** FATAL ERROR *** Entity #" << msg.get_id() << " shall be attached to #" << msg.get_parent_id()
				<< ", hook " << msg.get_parent_hook() << ", but parent doesn't exist in world."
				<< std::endl;
			;
			get_shared().lock_facility->lock_world( false );
			return;
		}

		// Check if entity is attached to invisible hook.
		if( *parent_ent->get_class().find_hook( msg.get_parent_hook() ) == fw::Class::INVISIBLE_HOOK ) {
			skip = true;
		}

		// Check if any parent entity is the player entity.
		while( parent_ent ) {
			if( parent_ent->get_id() == m_session_state->own_entity_id ) {
				// Check if entity is inventory. If so, use it to get contents.
				if( msg.get_parent_hook() == "inventory" ) {
					fw::msg::Use use_msg;

					use_msg.set_entity_id( msg.get_id() );
					get_shared().client->send_message( use_msg );

					// Create container for entity.
					m_user_interface->get_container_manager().create_container( *ent );
				}

				skip = true;
				break;
			}

			parent_ent = parent_ent->get_parent();
		}

		get_shared().lock_facility->lock_world( false );
	}

	if( !skip ) {
		std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( ms::string_hash( "entity_invalidated" ) );
		ms_message->set_property( ms::string_hash( "id" ), msg.get_id() );

		m_router->enqueue_message( ms_message );
	}
	*/

void SceneGraphReader::reset_nodes() {
	assert( m_session_state->current_planet_id.size() > 0 );

	// Detach before.
	if( m_planet_drawable ) {
		m_root_node->detach( m_planet_drawable );
	}

	if( m_entity_group_node ) {
		m_root_node->detach( m_entity_group_node );
	}

	// Reset nodes, no matter what.
	m_planet_drawable.reset();
	m_entity_group_node.reset();

	m_lock_facility->lock_world( true );

	// Fetch planet.
	const fw::Planet* planet = m_world->find_planet( m_session_state->current_planet_id );
	assert( planet != nullptr );

	m_lock_facility->lock_planet( *planet, true );
	m_lock_facility->lock_world( false );

	m_planet_drawable = PlanetDrawable::create( *planet, *m_resource_manager, *m_renderer );
	m_entity_group_node = EntityGroupNode::create( *m_resource_manager, *m_renderer );

	m_root_node->attach( m_planet_drawable );
	m_root_node->attach( m_entity_group_node );

	m_lock_facility->lock_planet( *planet, false );
}

void SceneGraphReader::prepare_loop() {
	std::cout << "SceneGraphReader: Prepare loop launched." << std::endl;

	boost::unique_lock<boost::mutex> condition_lock( m_prepare_condition_mutex );

	while( true ) {
		m_prepare_data_mutex.lock();

		// Terminate thread?
		if( m_terminate_prepare_thread == true ) {
			m_prepare_data_mutex.unlock();
			break;
		}

		// If there is nothing to prepare, wait.
		if( m_chunk_positions.size() == 0 && m_entities.size() == 0 ) {
			m_prepare_data_mutex.unlock();
			m_prepare_condition.wait( condition_lock );

			continue;
		}

		if( m_chunk_positions.size() > 0 ) {
			// Fetch next chunk, remove from list and prepare.
			fw::ChunkVector chunk_position = m_chunk_positions.front();

			m_chunk_positions.pop_front();
			m_prepare_data_mutex.unlock();

			// Cache planet drawable pointer. May be killed by a beam message.
			std::shared_ptr<PlanetDrawable> planet_drawable;

			{
				boost::lock_guard<boost::mutex> sg_lock( m_scene_graph_mutex );
				planet_drawable = m_planet_drawable;
			}

			// Lock the planet during the preparation.
			m_lock_facility->lock_world( true );

			const fw::Planet* planet = m_world->find_planet( m_session_state->current_planet_id );
			assert( planet != nullptr );

			m_lock_facility->lock_planet( *planet, true );
			m_lock_facility->lock_world( false );

			planet_drawable->prepare_chunk( chunk_position );

			m_lock_facility->lock_planet( *planet, false );
		}
		else if( m_entities.size() > 0 ) {
			// Fetch next entity ID, remove from list and prepare.
			fw::EntityID entity_id = m_entities.front();

			m_entities.pop_front();
			m_prepare_data_mutex.unlock();

			m_lock_facility->lock_world( true );

			const fw::Entity* entity = m_world->find_entity( entity_id );
			assert( entity != nullptr );

			m_entity_group_node->add_entity( *entity );

			m_lock_facility->lock_world( false );
		}

	}

	std::cout << "SceneGraphReader: Prepare loop terminating." << std::endl;
}
