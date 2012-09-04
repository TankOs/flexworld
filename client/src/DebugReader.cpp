#include "DebugReader.hpp"

#include <FlexWorld/Types.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <iostream>

DebugReader::DebugReader() :
	ms::Reader()
{
}

void DebugReader::handle_message( const ms::Message& /*message*/ ) {
}
