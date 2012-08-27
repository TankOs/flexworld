#include "SessionState.hpp"

SessionState::SessionState() :
	view_cuboid( 0, 0, 0, 0, 0, 0 ),
	current_planet_id( "" ),
	own_entity_id( 0 ),
	own_entity_received( false )
{
}
