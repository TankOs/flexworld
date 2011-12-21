#include <FlexWorld/Resource.hpp>
#include <FlexWorld/FlexID.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestResource ) {
	using namespace flex;

	// Create resource and check initial state.
	{
		FlexID id;
		id.parse( "fw.weapons/sword.png" );
		Resource resource( id );

		BOOST_CHECK( resource.get_id() == id );
		BOOST_CHECK( &resource.get_id() != &id );
	}
}

