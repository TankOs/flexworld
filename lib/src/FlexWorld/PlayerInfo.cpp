#include <FlexWorld/PlayerInfo.hpp>

namespace flex {

PlayerInfo::PlayerInfo() :
	view_cuboid( 0, 0, 0, 0, 0, 0 ),
	planet( nullptr ),
	local( false ),
	connected( false )
{
}

}
