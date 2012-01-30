#pragma once

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Cuboid.hpp>

namespace flex {

class Account;
class Planet;

/** Class for holding extra data for player connections.
 * Used by SessionHost.
 */
struct PlayerInfo {
	typedef Cuboid<Planet::ScalarType> ViewCuboid; ///< View cuboid.

	/** Ctor.
	 */
	PlayerInfo();

	ViewCuboid view_cuboid; ///< View range.
	Account* account; ///< Associated account.
	Planet* planet; ///< Associated planet.
	bool local; ///< Local connection?
	bool connected; ///< Connected?
};

}
