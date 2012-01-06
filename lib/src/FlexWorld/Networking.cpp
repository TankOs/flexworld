#include <FlexWorld/Networking.hpp>

namespace flex {

bool Networking::init() {
	#if defined( LINUX )
	#else
		WSADATA wsadata;

		if( WSAStartup( 0x0202, &wsadata ) != 0 ) {
			return false;
		}
	#endif

	return true;
}

void Networking::cleanup() {
	#if defined( LINUX )
	#else
		WSACleanup();
	#endif
}

}
