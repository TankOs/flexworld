#pragma once

#if defined( LINUX )
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#else
	#error NOT IMPLEMENTED
#endif

namespace flex {

/** Networking utitlities for easy cross-platform use.
 */
class Networking {
	public:
		/** Initialize networking sub-system.
		 * @return true on success.
		 */
		static bool init();

		/** Cleanup networking sub-system.
		 */
		static void cleanup();
};

}
