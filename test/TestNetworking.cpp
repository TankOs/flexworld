#include <FlexWorld/Networking.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestNetworking ) {
	using namespace flex;

	BOOST_REQUIRE( Networking::init() );
	Networking::cleanup();
}
