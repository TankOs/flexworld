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
		/** Dispatch SFML event to libRocket.
		 * @param event SFML event.
		 * @param context libRocket context.
		 */
		static void dispatch_event( const sf::Event& event, Rocket::Core::Context& context );
};
