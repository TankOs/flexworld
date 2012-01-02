#include <boost/test/unit_test.hpp>

/*BOOST_AUTO_TEST_CASE( TestMessage ) {
	//using namespace flex;

	// Define meta.
	MessageMeta meta;
	meta.add_field( MessageMeta::STRING );
	meta.add_field( MessageMeta::DWORD );
	meta.add_field( MessageMeta::WORD );
	meta.add_field( MessageMeta::BYTE );

	// Initial state.
	{
		Message msg( meta );

		BOOST_CHECK( &msg.get_meta() == &meta );
		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() );

		BOOST_CHECK( msg.get_string( 0 ) == "" );
		BOOST_CHECK( msg.get_dword( 1 ) == 0 );
		BOOST_CHECK( msg.get_word( 2 ) == 0 );
		BOOST_CHECK( msg.get_byte( 3 ) == 0 );
	}

	// Pack values.
	{
		Message msg( meta );

		msg.set_string( 0, "Test" );
		msg.set_dword( 1, 1234 );
		msg.set_word( 2, 123 );
		msg.set_byte( 3, 10 );

		BOOST_CHECK( msg.get_string( 0 ) == "Test" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );

		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 4 );
	}

	// Append string data.
	{
		Message msg( meta );

		msg.set_string( 0, "Test" );
		msg.set_dword( 1, 1234 );
		msg.set_word( 2, 123 );
		msg.set_byte( 3, 10 );

		msg.append_string( 0, "123" );
		msg.append_string( 0, "456" );
		msg.append_string( 0, "789" );

		BOOST_CHECK( msg.get_string( 0 ) == "Test123456789" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );

		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 4 + 3 + 3 + 3 );
	}
}*/
