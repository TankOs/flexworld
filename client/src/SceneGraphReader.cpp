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

static const ms::HashValue PLANET_ID_PROP_ID = ms::string_hash( "planet_id" );
static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );

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

void SceneGraphReader::set_world( const fw::World& world ) {
	m_world = &world;
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
		const fw::ChunkVector* chunk_position = message.find_property<fw::ChunkVector>( POSITION_PROP_ID );

		if( chunk_position ) {
			boost::lock_guard<boost::mutex> data_lock( m_prepare_data_mutex );
			m_chunk_positions.push_back( *chunk_position );

			m_prepare_condition.notify_one();
		}
	}
}

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
		if( m_chunk_positions.size() == 0 ) {
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

	}

	std::cout << "SceneGraphReader: Prepare loop terminating." << std::endl;
}
