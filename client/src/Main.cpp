#include "Client.hpp"

#ifdef LINUX
#include <X11/Xlib.h>
#endif

int main() {
#ifdef LINUX
	// Fix SFML multithreading bug.
	XInitThreads();
#endif

	Client app;

	app.run();

	return 0;
}
