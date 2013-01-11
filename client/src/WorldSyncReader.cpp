#include "WorldSyncReader.hpp"

#include <FlexWorld/Controllers/EntityWatchdog.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>

static const ms::HashValue ENTITY_CHANGE_ID = ms::string_hash( "entity_change" );
static const ms::HashValue FIELDS_ID = ms::string_hash( "fields" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue SNAPSHOT_ID = ms::string_hash( "snapshot" );

WorldSyncReader::WorldSyncReader() :
	ms::Reader(),
	m_world( nullptr ),
	m_lock_facility( nullptr )
{
}

void WorldSyncReader::set_world( fw::World& world ) {
	m_world = &world;
}

void WorldSyncReader::set_lock_facility( fw::LockFacility& lock_facility ) {
	m_lock_facility = &lock_facility;
}

void WorldSyncReader::handle_message( const ms::Message& message ) {
	assert( m_world != nullptr );
	assert( m_lock_facility != nullptr );

	auto msg_id = message.get_id();

	if( msg_id == ENTITY_CHANGE_ID ) {
		const auto* entity_id = message.find_property<fw::EntityID>( ID_ID );
		const auto* fields = message.find_property<int>( FIELDS_ID );
		const auto* const* snapshot = message.find_property<const fw::ctrl::EntityWatchdog::Snapshot*>( SNAPSHOT_ID );

		if( entity_id != nullptr && fields != nullptr && snapshot != nullptr ) {
			m_lock_facility->lock_world( true );

			auto entity = m_world->find_entity( *entity_id );
			assert( entity != nullptr );

			if( (*fields & fw::ctrl::EntityWatchdog::POSITION) == fw::ctrl::EntityWatchdog::POSITION ) {
				entity->set_position( (*snapshot)->position );
			}

			m_lock_facility->lock_world( false );
		}
	}
}
