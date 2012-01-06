#include <FlexWorld/Networking.hpp>

namespace flex {

bool Networking::init() {
	#if defined( LINUX )
		return true;
	#else
		#error NOT IMPLEMENTED
	#endif
}

void Networking::cleanup() {
	#if defined( LINUX )
	#else
		#error NOT IMPLEMENTED
	#endif
}

}
