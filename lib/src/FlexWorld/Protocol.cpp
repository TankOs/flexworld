#include <FlexWorld/Protocol.hpp>

#include <iostream>

namespace flex {

Protocol::~Protocol() {
}

void Protocol::handle_connect( ConnectionID /*id*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Connection not handled." << std::endl;
	#endif
}

void Protocol::handle_disconnect( ConnectionID /*id*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Disconnection not handled." << std::endl;
	#endif
}

std::size_t Protocol::handle_incoming_data( ConnectionID /*id*/, const Buffer& /*buffer*/ ) {
	#if !defined( NDEBUG )
		std::cerr << "WARNING: Incoming data not handled." << std::endl;
	#endif

	return 0;
}

}
