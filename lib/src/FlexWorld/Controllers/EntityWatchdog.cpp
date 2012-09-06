#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWCS/Entity.hpp>

namespace fw {
namespace ctrl {

EntityWatchdog::EntityWatchdog() :
	cs::Controller()
{
	listen_for<fw::EntityID>( "fw_entity_id" );
	listen_for<bool>( "watch" );
}

std::size_t EntityWatchdog::get_num_snapshots() const {
	return m_snapshots.size();
}

void EntityWatchdog::set_callback_function( const std::function<void( fw::EntityID, int )>& function ) {
	m_callback_function = function;
}

const std::function<void( fw::EntityID, int )>& EntityWatchdog::get_callback_function() const {
	return m_callback_function;
}

void EntityWatchdog::on_entity_add( cs::Entity& entity ) {
	const auto fw_entity_id = entity.find_property<fw::EntityID>( "fw_entity_id" );
	const auto position = entity.find_property<sf::Vector3f>( "position" );
	assert( fw_entity_id != nullptr );

	auto& snapshot = m_snapshots[fw_entity_id->get_value()];

	if( position != nullptr ) {
		snapshot.position = position->get_value();
	}
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

	// Call callback.
	if( m_callback_function && changed_fields != UNCHANGED ) {
		m_callback_function( fw_entity_id->get_value(), changed_fields );
	}
}

// ==================================================

bool EntityWatchdog::Snapshot::operator==( const Snapshot& other ) const {
	return
		position == other.position
	;
}

}
}
