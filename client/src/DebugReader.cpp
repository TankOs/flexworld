#include "DebugReader.hpp"

#include <FlexWorld/Types.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <iostream>

DebugReader::DebugReader() :
	ms::Reader()
{
}

void DebugReader::handle_message( const ms::Message& message ) {
	static const ms::HashValue CHUNK_INVALIDATED_ID = ms::string_hash( "chunk_invalidated" );
	static const ms::HashValue ENTITY_INVALIDATED_ID = ms::string_hash( "entity_invalidated" );
	static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );
	static const ms::HashValue ID_PROP_ID = ms::string_hash( "id" );

	const ms::HashValue message_id = message.get_id();

	//std::cout << "Message received: " << message_id << std::endl;

	if( message_id == CHUNK_INVALIDATED_ID ) {
		const fw::ChunkVector* chunk_position = message.find_property<fw::ChunkVector>( POSITION_PROP_ID );

		if( chunk_position ) {
			std::cout << "Chunk invalidated @ "
				<< static_cast<int>( chunk_position->x ) << " "
				<< static_cast<int>( chunk_position->y ) << " "
				<< static_cast<int>( chunk_position->z ) << " "
				<< "."
				<< std::endl
			;
		}
		else {
			std::cout << "chunk_invalidated message did not carry position info." << std::endl;
		}
	}
	else if( message_id == ENTITY_INVALIDATED_ID ) {
		const fw::EntityID* entity_id = message.find_property<fw::EntityID>( ID_PROP_ID );

		if( entity_id ) {
			std::cout << "Entity " << *entity_id << " invalidated." << std::endl;
		}
		else {
			std::cout << "entity_invalidated message did not carry entity ID info." << std::endl;
		}
	}
}
