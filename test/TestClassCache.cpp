#include <FlexWorld/ClassCache.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/FlexID.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestClassCache ) {
	using namespace flex;

	// Create cache and check initial state.
	{
		ClassCache cache;

		BOOST_CHECK( cache.get_num_cached_classes() == 0 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	FlexID class_id;
	class_id.parse( "fw.weapons/sword.yml" );

	Class cls( class_id );
	Class cls2( class_id );

	// Cache new class.
	{
		ClassCache cache;
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( &cache.get_class( 0 ) == &cls );
		BOOST_CHECK( cache.get_num_cached_classes() == 1 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Cache same class twice.
	{
		ClassCache cache;
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( &cache.get_class( 0 ) == &cls );
		BOOST_CHECK( cache.get_num_cached_classes() == 1 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Cache two classes.
	{
		ClassCache cache;
		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( cache.cache( cls2 ) == 1 );
		BOOST_CHECK( &cache.get_class( 0 ) == &cls );
		BOOST_CHECK( &cache.get_class( 1 ) == &cls2 );
		BOOST_CHECK( cache.get_num_cached_classes() == 2 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Forget without hole.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.forget( cls );

		BOOST_CHECK( cache.get_num_cached_classes() == 0 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Forget with hole.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.cache( cls2 );
		cache.forget( cls );

		BOOST_CHECK( cache.get_num_cached_classes() == 1 );
		BOOST_CHECK( cache.get_num_holes() == 1 );
		BOOST_CHECK( cache.is_id_valid( 0 ) == false );
	}

	// Fill hole.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.cache( cls2 );
		cache.forget( cls );

		BOOST_CHECK( cache.cache( cls ) == 0 );
		BOOST_CHECK( cache.get_num_cached_classes() == 2 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}

	// Clear.
	{
		ClassCache cache;
		cache.cache( cls );
		cache.cache( cls2 );
		cache.forget( cls );

		cache.clear();

		BOOST_CHECK( cache.get_num_cached_classes() == 0 );
		BOOST_CHECK( cache.get_num_holes() == 0 );
	}
}
