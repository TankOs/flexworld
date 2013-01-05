#pragma once

#include <FlexWorld/Planet.hpp>

#include <FWU/Cuboid.hpp>

namespace fw {

class Account;
class Planet;
class Entity;

/** Class for holding extra data for player connections.
 * Used by SessionHost.
 */
struct PlayerInfo {
	typedef util::Cuboid<Planet::ScalarType> ViewCuboid; ///< View cuboid.

	/** Ctor.
	 */
	PlayerInfo();

	ViewCuboid view_cuboid; ///< View range.
	std::string username; ///< Username.
	Entity* entity; ///< Associated entity.
	Planet* planet; ///< Associated planet.
	bool local; ///< Local connection?
	bool connected; ///< Connected?
};

}
