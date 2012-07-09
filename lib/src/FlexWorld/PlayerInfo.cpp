#include <FlexWorld/PlayerInfo.hpp>

namespace fw {

PlayerInfo::PlayerInfo() :
	view_cuboid( 0, 0, 0, 0, 0, 0 ),
	entity( nullptr ),
	planet( nullptr ),
	local( false ),
	connected( false )
{
}

}
