#include <FlexWorld/Peer.hpp>

namespace flex {

Peer::Peer() :
	id( 0 ),
	socket( nullptr )
{
}

Peer::Peer( ID id_, Socket& socket_ ) :
	id( id_ ),
	socket( &socket_ )
{
}

}
