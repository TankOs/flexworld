#include "HostSyncReader.hpp"

#include <FlexWorld/Cuboid.hpp>
#include <FlexWorld/Types.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Client.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>

static const ms::HashValue VIEW_CUBOID_UPDATE_ID = ms::string_hash( "view_cuboid_update" );
static const ms::HashValue CUBOID_ID = ms::string_hash( "cuboid" );

HostSyncReader::HostSyncReader() :
	ms::Reader(),
	m_client( nullptr )
{
}

void HostSyncReader::handle_message( const ms::Message& message ) {
	assert( m_client );

	ms::HashValue message_id = message.get_id();

	if( message_id == VIEW_CUBOID_UPDATE_ID ) {
		const fw::Cuboid<fw::PlanetSizeType>* cuboid = message.find_property<fw::Cuboid<fw::PlanetSizeType>>( CUBOID_ID );

		if( cuboid ) {
			std::cout << "HostSyncReader: View cuboid changed, requesting new chunks..." << std::endl;

			std::size_t num_requests = 0; // XXX
			fw::ChunkVector runner;

			for( runner.z = cuboid->z; runner.z < cuboid->z + cuboid->width; ++runner.z ) {
				for( runner.y = cuboid->y; runner.y < cuboid->y + cuboid->width; ++runner.y ) {
					for( runner.x = cuboid->x; runner.x < cuboid->x + cuboid->width; ++runner.x ) {
						fw::msg::RequestChunk req_msg;

						req_msg.set_position( runner );
						req_msg.set_timestamp( 0 );
						m_client->send_message( req_msg );

						++num_requests;
					}
				}
			}

			std::cout << "HostSyncReader: Requested " << num_requests << " chunks." << std::endl;
		}
	}
}

void HostSyncReader::set_client( fw::Client& client ) {
	m_client = &client;
}
