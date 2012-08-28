#include "MessageHandler.hpp"
#include "SessionState.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>

#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>

static const ms::HashValue BEAM_ID = ms::string_hash( "beam" );
static const ms::HashValue CHUNK_UPDATE_ID = ms::string_hash( "chunk_update" );

static const ms::HashValue PLANET_ID_PROP_ID = ms::string_hash( "planet_id" );
static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );

MessageHandler::MessageHandler(
	ms::Router& router
) :
	m_router( router )
{
}

void MessageHandler::handle_message( const fw::msg::Beam& msg, fw::Client::ConnectionID conn_id ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( BEAM_ID );

	ms_message->set_property( PLANET_ID_PROP_ID, msg.get_planet_name() );
	ms_message->set_property( POSITION_PROP_ID, msg.get_position() );

	m_router.enqueue_message( ms_message );

	/*
	// When being beamed, our own entity isn't there, so freeze movement until it has arrived.
	m_session_state.own_entity_received = false;

	// Fetch planet.
	m_lock_facility.lock_world( true );
	const fw::Planet* planet = m_world.find_planet( msg.get_planet_name() );

	if( planet ) {
		m_lock_facility.lock_planet( *planet, true );
	}

	m_lock_facility.lock_world( false );

	if( !planet ) {
		return;
	}

	// Save current planet ID.
	m_session_state.current_planet_id = msg.get_planet_name();

	// Update view cuboid.
	fw::ChunkVector chunk_pos;
	fw::BlockVector block_pos;

	if( !planet->transform( msg.get_position(), chunk_pos, block_pos ) ) {
		m_lock_facility.lock_planet( *planet, false );
		return;
	}

	m_session_state.view_cuboid.x = static_cast<fw::PlanetSizeType>( chunk_pos.x - std::min( chunk_pos.x, fw::Planet::ScalarType( 30 ) ) );
	m_session_state.view_cuboid.y = static_cast<fw::PlanetSizeType>( chunk_pos.y - std::min( chunk_pos.y, fw::Planet::ScalarType( 30 ) ) );
	m_session_state.view_cuboid.z = static_cast<fw::PlanetSizeType>( chunk_pos.z - std::min( chunk_pos.z, fw::Planet::ScalarType( 30 ) ) );
	m_session_state.view_cuboid.width = std::min( static_cast<fw::PlanetSizeType>( planet->get_size().x - chunk_pos.x ), fw::PlanetSizeType( 30 ) );
	m_session_state.view_cuboid.height = std::min( static_cast<fw::PlanetSizeType>( planet->get_size().y - chunk_pos.y ), fw::PlanetSizeType( 30 ) );
	m_session_state.view_cuboid.depth = std::min( static_cast<fw::PlanetSizeType>( planet->get_size().z - chunk_pos.z ), fw::PlanetSizeType( 30 ) );

	std::shared_ptr<ms::Message> reset_scenegraph_message = std::make_shared<ms::Message>( ms::string_hash( "reset_scenegraph" ) );
	reset_scenegraph_message->set_property( ms::string_hash( "reset_planet" ), planet->get_id() );

	m_lock_facility.lock_planet( *planet, false );
	*/

	/*
	// Detach old drawables.
	if( m_planet_drawable ) {
		m_scene_graph->detach( m_planet_drawable );
	}

	if( m_entity_group_node ) {
		m_scene_graph->detach( m_entity_group_node );
	}

	m_planet_drawable = PlanetDrawable::create( *planet, m_resource_manager, m_renderer );
	m_entity_group_node = EntityGroupNode::create( m_resource_manager, m_renderer );

	m_scene_graph->attach( m_planet_drawable );
	m_scene_graph->attach( m_entity_group_node );

	// Stop object preparer if running first.
	if( m_object_preparer_reader->is_running() ) {
		m_object_preparer_reader->stop();
	}

	// Update drawables.
	m_object_preparer_reader->set_planet_drawable( m_planet_drawable );
	m_object_preparer_reader->set_entity_group_node( m_entity_group_node );
	m_object_preparer_reader->launch();


	// Request chunks.
	request_chunks( m_session_state.view_cuboid );
	*/

}

void MessageHandler::handle_message( const fw::msg::ChunkUnchanged& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( CHUNK_UPDATE_ID );

	ms_message->set_property( POSITION_PROP_ID, msg.get_position() );

	m_router.enqueue_message( ms_message );

	/*
	if( !m_session_state->current_planet_id.size() ) {
		return;
	}

	get_shared().lock_facility->lock_world( true );
	const fw::Planet* planet = get_shared().world->find_planet( m_session_state->current_planet_id );

	if( !planet ) {
#if !defined( NDEBUG )
		std::cout << "WARNING: Host notified about an unchaned chunk at an invalid planet." << std::endl;
#endif
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}

	// Notify message system that we got a new chunk.
	if( m_object_preparer_reader ) {
		std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( ms::string_hash( "chunk_invalidated" ) );
		ms_message->set_property( ms::string_hash( "position" ), msg.get_position() );

		m_router.enqueue_message( ms_message );
	}
	*/
}
