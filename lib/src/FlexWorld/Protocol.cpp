#include <FlexWorld/Protocol.hpp>

#include <iostream>

namespace flex {

Protocol::~Protocol() {
}

void Protocol::handle_connect( Socket& /*socket*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Connection not handled." << std::endl;
	#endif
}

void Protocol::handle_disconnect( Socket& /*socket*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Disconnection not handled." << std::endl;
	#endif
}

std::size_t Protocol::handle_incoming_data( Socket& /*socket*/, const Buffer& /*buffer*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Incoming data not handled." << std::endl;
	#endif

	return 0;
}

}
