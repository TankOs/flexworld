#include "CameraReader.hpp"

#include <FWMS/Message.hpp>
#include <cassert>
#include <iostream> // XXX 

static const ms::HashValue CONTROL_ENTITY_ID = ms::string_hash( "control_entity" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );

CameraReader::CameraReader() :
	ms::Reader(),
	m_camera( nullptr ),
	m_world( nullptr ),
	m_entity_id( 0 ),
	m_received_entity_id( false )
{
}

void CameraReader::handle_message( const ms::Message& message ) {
	assert( m_camera != nullptr );
	assert( m_world != nullptr );

	ms::HashValue message_id = message.get_id();

	if( message_id == CONTROL_ENTITY_ID ) {
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
