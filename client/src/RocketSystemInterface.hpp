#pragma once

#include <Rocket/Core.h>
#include <SFML/System/Clock.hpp>

/** System interface for libRocket.
 */
class RocketSystemInterface : public Rocket::Core::SystemInterface {
	public:
		float GetElapsedTime();

	private:
		sf::Clock m_timer;
};
