#include "RocketSystemInterface.hpp"

float RocketSystemInterface::GetElapsedTime() {
	return m_timer.getElapsedTime().asSeconds();
}
