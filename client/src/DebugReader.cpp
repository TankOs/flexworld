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
	static const ms::HashValue CHUNK_CREATED_ID = ms::string_hash( "chunk_created" );
	static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );

	const ms::HashValue message_id = message.get_id();

	//std::cout << "Message received: " << message_id << std::endl;

	if( message_id == CHUNK_CREATED_ID ) {
		const fw::ChunkVector* chunk_position = message.find_property<fw::ChunkVector>( POSITION_PROP_ID );

		if( chunk_position ) {
			std::cout << "New chunk created @ "
				<< static_cast<int>( chunk_position->x ) << " "
				<< static_cast<int>( chunk_position->y ) << " "
				<< static_cast<int>( chunk_position->z ) << " "
				<< "."
				<< std::endl
			;
		}
		else {
			std::cout << "chunk_created message did not carry position info." << std::endl;
		}
	}
}
