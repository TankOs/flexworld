#include "MessageHandler.hpp"
#include "SessionState.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>

#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>

static const ms::HashValue BEAM_ID = ms::string_hash( "beam" );
static const ms::HashValue CHUNK_UPDATE_ID = ms::string_hash( "chunk_update" );
static const ms::HashValue CLASS_ID = ms::string_hash( "class" );
static const ms::HashValue CREATE_ENTITY_ID = ms::string_hash( "create_entity" );
static const ms::HashValue HEADING_ID = ms::string_hash( "heading" );
static const ms::HashValue HOOK_ID = ms::string_hash( "hook" );
static const ms::HashValue ID_ID = ms::string_hash( "id" );
static const ms::HashValue PARENT_ID_ID = ms::string_hash( "parent_id" );
static const ms::HashValue PLANET_ID_ID = ms::string_hash( "planet_id" );
static const ms::HashValue POSITION_ID = ms::string_hash( "position" );

MessageHandler::MessageHandler(
	ms::Router& router
) :
	m_router( router )
{
}

void MessageHandler::handle_message( const fw::msg::Beam& msg, fw::Client::ConnectionID conn_id ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( BEAM_ID );

	ms_message->set_property( PLANET_ID_ID, msg.get_planet_name() );
	ms_message->set_property( POSITION_ID, msg.get_position() );

	m_router.enqueue_message( ms_message );
}

void MessageHandler::handle_message( const fw::msg::ChunkUnchanged& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( CHUNK_UPDATE_ID );

	ms_message->set_property( POSITION_ID, msg.get_position() );

	m_router.enqueue_message( ms_message );
}

void MessageHandler::handle_message( const fw::msg::CreateEntity& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( CREATE_ENTITY_ID );

	ms_message->set_property( ID_ID, msg.get_id() );
	ms_message->set_property( CLASS_ID, msg.get_class() );
	ms_message->set_property( POSITION_ID, msg.get_position() );
	ms_message->set_property( HEADING_ID, msg.get_heading() );
	ms_message->set_property( PARENT_ID_ID, msg.get_parent_id() );
	ms_message->set_property( HOOK_ID, msg.get_parent_hook() );

	m_router.enqueue_message( ms_message );
}
