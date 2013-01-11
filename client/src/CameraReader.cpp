#include "CameraReader.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Controllers/EntityWatchdog.hpp>
#include <FlexWorld/World.hpp>

#include <FWMS/Message.hpp>
#include <FWSG/Transform.hpp>
#include <FWSG/Camera.hpp>
#include <cassert>
#include <iostream> // XXX 

static const ms::HashValue CONTROL_ENTITY_ID = ms::string_hash( "control_entity" );
static const ms::HashValue ENTITY_CHANGE_ID = ms::string_hash( "entity_change" );
static const ms::HashValue FIELDS_ID = ms::string_hash( "fields" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue SNAPSHOT_ID = ms::string_hash( "snapshot" );

CameraReader::CameraReader() :
	ms::Reader(),
	m_camera( nullptr ),
	m_world( nullptr ),
	m_lock_facility( nullptr ),
	m_entity_id( 0 ),
	m_received_entity_id( false )
{
}

void CameraReader::handle_message( const ms::Message& message ) {
	assert( m_camera != nullptr );
	assert( m_world != nullptr );
	assert( m_lock_facility != nullptr );

	ms::HashValue message_id = message.get_id();

	if( message_id == ENTITY_CHANGE_ID ) {
		// Process only if controlling an entity.
		if( m_received_entity_id ) {
			const auto* entity_id = message.find_property<fw::EntityID>( ID_ID );
			const auto* fields = message.find_property<int>( FIELDS_ID );
			const auto* const* snapshot = message.find_property<const fw::ctrl::EntityWatchdog::Snapshot*>( SNAPSHOT_ID );

			if( entity_id != nullptr && *entity_id == m_entity_id && fields != nullptr && snapshot != nullptr ) {
				if( *fields & fw::ctrl::EntityWatchdog::POSITION ) {
					// Prepare transform object.
					sg::Transform transform = m_camera->get_transform();

					m_lock_facility->lock_world( true );

					// Get entity and apply position.
					auto entity = m_world->find_entity( *entity_id );
					assert( entity != nullptr );

					transform.set_translation( entity->get_position() );

					m_lock_facility->lock_world( false );

					// Apply new transform.
					m_camera->set_transform( transform );
				}
			}
		}
	}
	else if( message_id == CONTROL_ENTITY_ID ) {
		const fw::EntityID* entity_id = message.find_property<fw::EntityID>( ID_ID );

		if( entity_id != nullptr ) {
			m_received_entity_id = true;
			m_entity_id = *entity_id;
		}

		std::cout << "CameraReader: Now controlling entity #" << m_entity_id << "." << std::endl;
	}

	if( m_received_entity_id == false ) {
		return;
	}
}

void CameraReader::set_camera( sg::Camera& camera ) {
	m_camera = &camera;
}

void CameraReader::set_world( const fw::World& world ) {
	m_world = &world;
}

void CameraReader::set_lock_facility( fw::LockFacility& lock_facility ) {
	m_lock_facility = &lock_facility;
}
