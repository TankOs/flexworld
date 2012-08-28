#include "MessageHandler.hpp"
#include "SessionState.hpp"

#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>

#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>

static const ms::HashValue BEAM_ID = ms::string_hash( "beam" );
static const ms::HashValue CHUNK_UPDATE_ID = ms::string_hash( "chunk_update" );

static const ms::HashValue PLANET_ID_PROP_ID = ms::string_hash( "planet_id" );
static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );

MessageHandler::MessageHandler(
	ms::Router& router
) :
	m_router( router )
{
}

void MessageHandler::handle_message( const fw::msg::Beam& msg, fw::Client::ConnectionID conn_id ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( BEAM_ID );

	ms_message->set_property( PLANET_ID_PROP_ID, msg.get_planet_name() );
	ms_message->set_property( POSITION_PROP_ID, msg.get_position() );

	m_router.enqueue_message( ms_message );
}

void MessageHandler::handle_message( const fw::msg::ChunkUnchanged& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( CHUNK_UPDATE_ID );

	ms_message->set_property( POSITION_PROP_ID, msg.get_position() );

	m_router.enqueue_message( ms_message );
}
