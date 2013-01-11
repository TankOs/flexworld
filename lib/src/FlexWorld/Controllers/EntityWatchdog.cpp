#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWCS/Entity.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <iostream>
#include <cassert>

namespace fw {
namespace ctrl {

static const ms::HashValue ENTITY_CHANGE_ID = ms::string_hash( "entity_change" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue FIELDS_ID = ms::string_hash( "fields" );
static const ms::HashValue SNAPSHOT_ID = ms::string_hash( "snapshot" );

const cs::ControllerRequirements& EntityWatchdog::get_requirements() {
	static const auto req = cs::ControllerRequirements(
	).require_property<fw::EntityID>(
		"fw_entity_id", true
	).require_property<ms::Router*>(
		"watch_router", true
	).require_property<sf::Vector3f>(
		"position", true
	);

	return req;
}

EntityWatchdog::EntityWatchdog( cs::Entity& entity ) :
	cs::Controller( entity ),
	m_fw_entity_id{ entity.find_property<fw::EntityID>( "fw_entity_id" ) },
	m_watch_router{ entity.find_property<ms::Router*>( "watch_router" ) },
	m_position{ entity.find_property<sf::Vector3f>( "position" ) }
{
	assert( m_fw_entity_id != nullptr );
	assert( m_watch_router != nullptr );
	assert( m_position != nullptr );

#ifndef NDEBUG
	std::cout << "EntityWatchdog controller attached to entity #" << entity.get_id() << std::endl;
#endif

	// Send initial change message.
	m_snapshot.position = *m_position;

	auto entity_change_message = std::make_shared<ms::Message>( ENTITY_CHANGE_ID );
	entity_change_message->set_property<fw::EntityID>( ID_ID, *m_fw_entity_id );
	entity_change_message->set_property<int>( FIELDS_ID, ChangeFieldFlag::ALL );
	entity_change_message->set_property<const Snapshot*>( SNAPSHOT_ID, &m_snapshot );

	(*m_watch_router)->enqueue_message( entity_change_message );
}

void EntityWatchdog::execute( const sf::Time& sim_time ) {
	int changed_fields{ ChangeFieldFlag::UNCHANGED };

	// Check for position change.
	/*
	std::cout << "Snapshot: "
		<< m_snapshot.position.x << ", "
		<< m_snapshot.position.y << ", "
		<< m_snapshot.position.z << ", "
		<< std::endl
	;
	std::cout << "New position: "
		<< m_position->x << ", "
		<< m_position->y << ", "
		<< m_position->z << ", "
		<< std::endl
	;
	*/

	if( m_snapshot.position != *m_position ) {
		changed_fields |= ChangeFieldFlag::POSITION;
		m_snapshot.position = *m_position;
	}

	// If anything was changed, enqueue message.
	if( changed_fields != ChangeFieldFlag::UNCHANGED ) {
		auto entity_change_message = std::make_shared<ms::Message>( ENTITY_CHANGE_ID );

		entity_change_message->set_property<fw::EntityID>( ID_ID, *m_fw_entity_id );
		entity_change_message->set_property<int>( FIELDS_ID, changed_fields );
		entity_change_message->set_property<const Snapshot*>( SNAPSHOT_ID, &m_snapshot );

		(*m_watch_router)->enqueue_message( entity_change_message );
	}
}

}
}
