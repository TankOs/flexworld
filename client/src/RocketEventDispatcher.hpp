#pragma once

namespace sf {
class Event;
}

namespace Rocket {
namespace Core {
class Context;
}
}

/** Dispatcher for sending SFML events to a libRocket context.
 */
class RocketEventDispatcher {
	public:
		static void dispatch_event( const sf::Event& event, Rocket::Core::Context& context );
};
