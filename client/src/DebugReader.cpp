#include "DebugReader.hpp"
#include "Shared.hpp"

#include <FlexWorld/Types.hpp>
#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <iostream>

DebugReader::DebugReader() :
	ms::Reader(),
	m_player_position{ 0.0f, 0.0f, 0.0f }
{
}

void DebugReader::handle_message( const ms::Message& message ) {
	using fw::ctrl::EntityWatchdog;

	static const ms::HashValue ENTITY_CHANGE_ID = ms::string_hash( "entity_change" );
	static const ms::HashValue ID_ID = ms::string_hash( "id" );
	static const ms::HashValue FIELDS_ID = ms::string_hash( "fields" );
	static const ms::HashValue SNAPSHOT_ID = ms::string_hash( "snapshot" );

	if( message.get_id() == ENTITY_CHANGE_ID ) {
		const auto* entity_id = message.find_property<fw::EntityID>( ID_ID );
		const auto* fields = message.find_property<int>( FIELDS_ID );
		const auto* const* snapshot = message.find_property<const EntityWatchdog::Snapshot*>( SNAPSHOT_ID );

		if(
			entity_id != nullptr &&
			fields != nullptr &&
			snapshot != nullptr
		) {
			if(
				*entity_id == get_shared().entity_id &&
				(*fields & EntityWatchdog::ChangeFieldFlag::POSITION) == EntityWatchdog::ChangeFieldFlag::POSITION
			) {
				m_player_position = (*snapshot)->position;
			}
		}
	}
}

const sf::Vector3f& DebugReader::get_player_position() const {
	return m_player_position;
}
