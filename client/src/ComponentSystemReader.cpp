#include "ComponentSystemReader.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>
#include <FlexWorld/Entity.hpp>

#include <FWCS/System.hpp>
#include <FWCS/Entity.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream> // XXX 
#include <cassert>

static const ms::HashValue WALK_ID = ms::string_hash( "walk" );
static const ms::HashValue VECTOR_ID = ms::string_hash( "vector" );
static const ms::HashValue CREATE_ENTITY_ID = ms::string_hash( "create_entity" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue CONTROL_ENTITY_ID = ms::string_hash( "control_entity" );

ComponentSystemReader::ComponentSystemReader() :
	ms::Reader(),
	m_system( nullptr ),
	m_world( nullptr ),
	m_lock_facility( nullptr ),
	m_controlled_entity_id( 0 ),
	m_controlling_entity( false )
{
}

void ComponentSystemReader::set_system( cs::System& system ) {
	m_system = &system;
}

void ComponentSystemReader::handle_message( const ms::Message& message ) {
	assert( get_router() != nullptr );
	assert( m_system != nullptr );
	assert( m_world != nullptr );
	assert( m_lock_facility != nullptr );

	auto msg_id = message.get_id();

	if( msg_id == CREATE_ENTITY_ID ) {
		const auto entity_id = message.find_property<fw::EntityID>( ID_ID );

		if( entity_id != nullptr ) {
			assert( m_entities.find( *entity_id ) == std::end( m_entities ) );

			// Create CS entity.
			cs::Entity* cs_entity = &m_system->create_entity();
			m_entities[*entity_id] = cs_entity;
			std::cout << "ComponentSystemReader: Entity " << *entity_id << " added to system." << std::endl;
		}
	}
	else if( msg_id == CONTROL_ENTITY_ID ) {
		auto entity_id = message.find_property<fw::EntityID>( ID_ID );

		if( entity_id != nullptr ) {
			// TODO: Remove properties from old controlled entity.
			m_controlled_entity_id = *entity_id;
			m_controlling_entity = true;

			// Create needed properties for controlled entity.
			auto ent_iter = m_entities.find( *entity_id );
			assert( ent_iter != std::end( m_entities ) );

			// Get original entity to set properties.
			m_lock_facility->lock_world( true );

			const auto* entity = m_world->find_entity( *entity_id );
			assert( entity != nullptr );

			ent_iter->second->create_property( "force", sf::Vector3f( 0.0f, 0.0f, 0.0f ) );
			ent_iter->second->create_property( "acceleration", sf::Vector3f( 0.0f, 0.0f, 0.0f ) );
			ent_iter->second->create_property( "velocity", sf::Vector3f( 0.0f, 0.0f, 0.0f ) );
			ent_iter->second->create_property( "position", entity->get_position() );
			ent_iter->second->create_property( "forward_vector", sf::Vector3f( 0.0f, 0.0f, -1.0f ) );
			ent_iter->second->create_property( "walk_control_vector", sf::Vector2f( 0.0f, 0.0f ) );
			ent_iter->second->create_property( "walk_force", 500.0f );
			ent_iter->second->create_property( "max_walk_velocity", 5.0f );
			ent_iter->second->create_property( "mass", 60.0f );
			ent_iter->second->create_property( "watch", true );
			ent_iter->second->create_property<fw::EntityID>( "fw_entity_id", *entity_id );

			m_lock_facility->lock_world( false );

			std::cout << "ComponentSystemReader: Now controlling entity #" << *entity_id << "." << std::endl;
		}
	}
	else if( msg_id == WALK_ID ) {
		const auto vector = message.find_property<sf::Vector2f>( VECTOR_ID );

		if( vector != nullptr && m_controlling_entity == true ) {
			auto controlled_entity = m_entities.find( m_controlled_entity_id )->second;
			controlled_entity->find_property<sf::Vector2f>( "walk_control_vector" )->set_value( *vector );

			std::cout << "ComponentSystemReader: Updated walk vector for entity #" << m_controlled_entity_id << "." << std::endl;
		}
	}
}

void ComponentSystemReader::set_world( const fw::World& world ) {
	m_world = &world;
}

void ComponentSystemReader::set_lock_facility( fw::LockFacility& lock_facility ) {
	m_lock_facility = &lock_facility;
}

const cs::Entity* ComponentSystemReader::get_controlled_entity() const {
	return m_controlling_entity == false ? nullptr : m_entities.find( m_controlled_entity_id )->second;
}
