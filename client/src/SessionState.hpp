#pragma once

#include <FlexWorld/Types.hpp>
#include <FlexWorld/Cuboid.hpp>

#include <string>

/** Session state.
 * The session state contains stateful information for one gaming session.
 */
struct SessionState {
	/** Ctor.
	 */
	SessionState();

	fw::Cuboid<fw::PlanetSizeType> view_cuboid; ///< View cuboid.
	std::string current_planet_id; ///< Current planet's ID.
	fw::EntityID own_entity_id; ///< Own entity ID.
	bool own_entity_received; ///< true when own entity got received.
};
