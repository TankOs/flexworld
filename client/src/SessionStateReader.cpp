#include "SessionStateReader.hpp"
#include "SessionState.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>
#include <FlexWorld/Planet.hpp>

#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <cassert>
#include <iostream>

static const ms::HashValue BEAM_ID = ms::string_hash( "beam" );
static const ms::HashValue VIEW_CUBOID_UPDATE_ID = ms::string_hash( "view_cuboid_update" );
static const ms::HashValue CREATE_ENTITY_ID = ms::string_hash( "create_entity" );
static const ms::HashValue CONTROL_ENTITY_ID = ms::string_hash( "control_entity" );

static const ms::HashValue PLANET_ID_ID = ms::string_hash( "planet_id" );
static const ms::HashValue POSITION_ID = ms::string_hash( "position" );
static const ms::HashValue CUBOID_ID = ms::string_hash( "cuboid" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );

SessionStateReader::SessionStateReader() :
	ms::Reader(),
	m_session_state( nullptr ),
	m_world( nullptr ),
	m_lock_facility( nullptr )
{
}

void SessionStateReader::set_session_state( SessionState& session_state ) {
	m_session_state = &session_state;
}

void SessionStateReader::handle_message( const ms::Message& message ) {
	assert( m_session_state != nullptr );
	assert( m_world != nullptr );
	assert( m_lock_facility != nullptr );
	assert( get_router() );

	if( message.get_id() == BEAM_ID ) {
		const std::string* planet_id = message.find_property<std::string>( PLANET_ID_ID );
		const fw::PlanetVector* position = message.find_property<fw::PlanetVector>( POSITION_ID );

		if( planet_id && position ) {
			// Set current planet ID.
			m_session_state->current_planet_id = *planet_id;

			// Reset own entity received flag.
			m_session_state->own_entity_received = false;

			// Get planet and lock.
			m_lock_facility->lock_world( true );

			const fw::Planet* planet = m_world->find_planet( *planet_id );
			assert( planet != nullptr );

			m_lock_facility->lock_planet( *planet, true );
			m_lock_facility->lock_world( false );

			// Transform position and update view cuboid.
			fw::ChunkVector chunk_pos;
			fw::BlockVector block_pos;

			bool result = planet->transform( *position, chunk_pos, block_pos );
			assert( result );

			if( !result ) {
				std::cout << "WARNING: SessionStateReader: Host sent an invalid position, view cuboid might be corrupt." << std::endl;
			}

			m_lock_facility->lock_planet( *planet, false );

			m_session_state->view_cuboid = util::Cuboid<fw::PlanetSizeType>(
				static_cast<fw::PlanetSizeType>( chunk_pos.x - std::min( chunk_pos.x, fw::PlanetSizeType( 30 ) ) ),
				static_cast<fw::PlanetSizeType>( chunk_pos.y - std::min( chunk_pos.y, fw::PlanetSizeType( 30 ) ) ),
				static_cast<fw::PlanetSizeType>( chunk_pos.z - std::min( chunk_pos.z, fw::PlanetSizeType( 30 ) ) ),
				std::min( static_cast<fw::PlanetSizeType>( planet->get_size().x - chunk_pos.x ), fw::PlanetSizeType( 30 ) ),
				std::min( static_cast<fw::PlanetSizeType>( planet->get_size().y - chunk_pos.y ), fw::PlanetSizeType( 30 ) ),
				std::min( static_cast<fw::PlanetSizeType>( planet->get_size().z - chunk_pos.z ), fw::PlanetSizeType( 30 ) )
			);

			// Send message to notify readers about the view cuboid change.
			std::shared_ptr<ms::Message> vc_message = std::make_shared<ms::Message>( VIEW_CUBOID_UPDATE_ID );
			vc_message->set_property( CUBOID_ID, m_session_state->view_cuboid );

			get_router()->enqueue_message( vc_message );

			std::cout << "SessionStateReader: Current planet ID updated to " << *planet_id << "." << std::endl;
			std::cout << "SessionStateReader: View cuboid updated." << std::endl;
		}
	}
	else if( message.get_id() == CREATE_ENTITY_ID ) {
		const fw::EntityID* entity_id = message.find_property<fw::EntityID>( ID_ID );

		if( entity_id && *entity_id == m_session_state->own_entity_id ) {
			m_session_state->own_entity_received = true;
			std::cout << "SessionStateReader: Own entity received." << std::endl;

			// TODO: This is going to be removed when the new ControlEntity network
			// will be added.
			auto control_message = std::make_shared<ms::Message>( ms::string_hash( "control_entity" ) );
			control_message->set_property<fw::EntityID>( ms::string_hash( "id" ), *entity_id );

			get_router()->enqueue_message( control_message );

			std::cout << "SessionStateReader: SENT OUT control_entity MESSAGE, REPLACE BY ControlEntity NETWORK MESSAGE." << std::endl;
		}
	}
}

void SessionStateReader::set_world( const fw::World& world ) {
	m_world = &world;
}

void SessionStateReader::set_lock_facility( fw::LockFacility& lock_facility ) {
	m_lock_facility = &lock_facility;
}
