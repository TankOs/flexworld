#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWCS/Entity.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>

namespace fw {
namespace ctrl {

static const ms::HashValue ENTITY_CHANGE_ID = ms::string_hash( "entity_change" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue FIELDS_ID = ms::string_hash( "fields" );
static const ms::HashValue SNAPSHOT_ID = ms::string_hash( "snapshot" );

EntityWatchdog::EntityWatchdog() :
	cs::Controller(),
	m_router( nullptr )
{
	listen_for<fw::EntityID>( "fw_entity_id" );
	listen_for<bool>( "watch" );
}

std::size_t EntityWatchdog::get_num_snapshots() const {
	return m_snapshots.size();
}

void EntityWatchdog::on_entity_add( cs::Entity& entity ) {
	assert( m_router != nullptr );

	auto fw_entity_id = entity.find_property<fw::EntityID>( "fw_entity_id" );
	auto position = entity.find_property<sf::Vector3f>( "position" );
	assert( fw_entity_id != nullptr );

	auto& snapshot = m_snapshots[fw_entity_id->get_value()];

	if( position != nullptr ) {
		snapshot.position = position->get_value();
	}

	// Send initial change message.
	auto entity_change_message = std::make_shared<ms::Message>( ENTITY_CHANGE_ID );

	entity_change_message->set_property<fw::EntityID>( ID_ID, fw_entity_id->get_value() );
	entity_change_message->set_property<int>( FIELDS_ID, ALL );
	entity_change_message->set_property<Snapshot>( SNAPSHOT_ID, snapshot );

	m_router->enqueue_message( entity_change_message );
}

void EntityWatchdog::on_entity_remove( cs::Entity& entity ) {
	const auto fw_entity_id = entity.find_property<fw::EntityID>( "fw_entity_id" );
	assert( fw_entity_id != nullptr );
	assert( find_snapshot( fw_entity_id->get_value() ) != nullptr );

	m_snapshots.erase( fw_entity_id->get_value() );
}

const EntityWatchdog::Snapshot* EntityWatchdog::find_snapshot( fw::EntityID entity_id ) const {
	const auto snapshot_iter = m_snapshots.find( entity_id );
	return snapshot_iter == std::end( m_snapshots ) ? nullptr : &snapshot_iter->second;
}

void EntityWatchdog::update_entity( cs::Entity& entity, const sf::Time& /*delta*/ ) {
	assert( m_router != nullptr );

	auto fw_entity_id = entity.find_property<fw::EntityID>( "fw_entity_id" );
	assert( fw_entity_id != nullptr );
	assert( m_snapshots.find( fw_entity_id->get_value() ) != std::end( m_snapshots ) );

	auto& snapshot = m_snapshots.find( fw_entity_id->get_value() )->second;
	auto position = entity.find_property<sf::Vector3f>( "position" );
	int changed_fields = UNCHANGED;

	if( position != nullptr ) {
		if( position->get_value() != snapshot.position ) {
			snapshot.position = position->get_value();
			changed_fields |= POSITION;
		}
	}

	// Send message if anything changed.
	if( changed_fields != UNCHANGED ) {
		auto entity_change_message = std::make_shared<ms::Message>( ENTITY_CHANGE_ID );

		entity_change_message->set_property<fw::EntityID>( ID_ID, fw_entity_id->get_value() );
		entity_change_message->set_property<int>( FIELDS_ID, changed_fields );
		entity_change_message->set_property<Snapshot>( SNAPSHOT_ID, snapshot );

		m_router->enqueue_message( entity_change_message );
	}
}

void EntityWatchdog::set_router( ms::Router& router ) {
	m_router = &router;
}

ms::Router* EntityWatchdog::get_router() const {
	return m_router;
}

// ==================================================

bool EntityWatchdog::Snapshot::operator==( const Snapshot& other ) const {
	return
		position == other.position
	;
}

}
}
