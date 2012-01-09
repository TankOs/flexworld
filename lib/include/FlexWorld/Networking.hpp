#pragma once

namespace flex {

static const int MAX_CONNECTIONS = 512; ///< Maximum allowed connections for the server (limited by FD_SETSIZE, which is set to this value).

}

#if defined( LINUX )
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#else
	#define FD_SETSIZE MAX_CONNECTIONS
	#include <WinSock2.h>
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
