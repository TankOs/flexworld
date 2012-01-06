#include <FlexWorld/Peer.hpp>
#include <FlexWorld/Socket.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestPeer ) {
	using namespace flex;

	// Initial state.
	{
		Peer peer;

		BOOST_CHECK( peer.socket == nullptr );
		BOOST_CHECK( peer.id == 0 );
		BOOST_CHECK( peer.buffer.size() == 0 );
	}

	// Initial with different ctor.
	{
		Socket dummy;
		Peer peer( 5, dummy );

		BOOST_CHECK( peer.socket == &dummy );
		BOOST_CHECK( peer.id == 5 );
		BOOST_CHECK( peer.buffer.size() == 0 );
	}
}
