#include <boost/test/unit_test.hpp>

#include <FlexWorld/Message.hpp>

BOOST_AUTO_TEST_CASE( TestMessage ) {
	using namespace flex;

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

		BOOST_CHECK( msg.get_string_length( 0 ) == 0 );
		BOOST_CHECK( msg.get_string( 0 ) == "" );
		BOOST_CHECK( msg.get_dword( 1 ) == 0 );
		BOOST_CHECK( msg.get_word( 2 ) == 0 );
		BOOST_CHECK( msg.get_byte( 3 ) == 0 );
	}

	// Set values.
	{
		Message msg( meta );

		msg.set_string( 0, "Test" );
		msg.set_dword( 1, 1234 );
		msg.set_word( 2, 123 );
		msg.set_byte( 3, 10 );

		BOOST_CHECK( msg.get_string_length( 0 ) == 4 );
		BOOST_CHECK( msg.get_string( 0 ) == "Test" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );

		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 4 );
	}

	// Set different strings to test enlarging/shrinking the buffer.
	{
		Message msg( meta );

		msg.set_dword( 1, 1234 );
		msg.set_word( 2, 123 );
		msg.set_byte( 3, 10 );

		msg.set_string( 0, "Test" );
		BOOST_CHECK( msg.get_string_length( 0 ) == 4 );
		BOOST_CHECK( msg.get_string( 0 ) == "Test" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );
		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 4 );

		// Enlarge again.
		msg.set_string( 0, "TestTest" );
		BOOST_CHECK( msg.get_string_length( 0 ) == 8 );
		BOOST_CHECK( msg.get_string( 0 ) == "TestTest" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );
		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 8 );

		// Shrink.
		msg.set_string( 0, "Foo" );
		BOOST_CHECK( msg.get_string_length( 0 ) == 3 );
		BOOST_CHECK( msg.get_string( 0 ) == "Foo" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );
		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 3 );

		// Shrink again.
		msg.set_string( 0, "" );
		BOOST_CHECK( msg.get_string_length( 0 ) == 0 );
		BOOST_CHECK( msg.get_string( 0 ) == "" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );
		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() );

		// Enlarge.
		msg.set_string( 0, "Final" );
		BOOST_CHECK( msg.get_string_length( 0 ) == 5 );
		BOOST_CHECK( msg.get_string( 0 ) == "Final" );
		BOOST_CHECK( msg.get_dword( 1 ) == 1234 );
		BOOST_CHECK( msg.get_word( 2 ) == 123 );
		BOOST_CHECK( msg.get_byte( 3 ) == 10 );
		BOOST_CHECK( msg.get_size() == meta.get_minimum_size() + 5 );

	}

	// Do the same with string at second position.
	{
		MessageMeta mid_meta;
		mid_meta.add_field( MessageMeta::DWORD );
		mid_meta.add_field( MessageMeta::STRING );
		mid_meta.add_field( MessageMeta::DWORD );

		Message msg( mid_meta );
		msg.set_dword( 0, 111 );
		msg.set_dword( 2, 333 );
		msg.set_string( 1, "Foo" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_string_length( 1 ) == 3 );
		BOOST_CHECK( msg.get_string( 1 ) == "Foo" );
		BOOST_CHECK( msg.get_dword( 2 ) == 333 );
		BOOST_CHECK( msg.get_size() == mid_meta.get_minimum_size() + 3 );

		// Enlarge.
		msg.set_string( 1, "FooBar" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_string_length( 1 ) == 6 );
		BOOST_CHECK( msg.get_string( 1 ) == "FooBar" );
		BOOST_CHECK( msg.get_dword( 2 ) == 333 );
		BOOST_CHECK( msg.get_size() == mid_meta.get_minimum_size() + 6 );

		// Enlarge again.
		msg.set_string( 1, "FooBarBaz" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_string_length( 1 ) == 9 );
		BOOST_CHECK( msg.get_string( 1 ) == "FooBarBaz" );
		BOOST_CHECK( msg.get_dword( 2 ) == 333 );
		BOOST_CHECK( msg.get_size() == mid_meta.get_minimum_size() + 9 );

		// Shrink.
		msg.set_string( 1, "Bleh" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_string_length( 1 ) == 4 );
		BOOST_CHECK( msg.get_string( 1 ) == "Bleh" );
		BOOST_CHECK( msg.get_dword( 2 ) == 333 );
		BOOST_CHECK( msg.get_size() == mid_meta.get_minimum_size() + 4 );

		// Shrink again.
		msg.set_string( 1, "" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_string_length( 1 ) == 0 );
		BOOST_CHECK( msg.get_string( 1 ) == "" );
		BOOST_CHECK( msg.get_dword( 2 ) == 333 );
		BOOST_CHECK( msg.get_size() == mid_meta.get_minimum_size() );

		// Enlarge again.
		msg.set_string( 1, "DaFinal" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_string_length( 1 ) == 7 );
		BOOST_CHECK( msg.get_string( 1 ) == "DaFinal" );
		BOOST_CHECK( msg.get_dword( 2 ) == 333 );
		BOOST_CHECK( msg.get_size() == mid_meta.get_minimum_size() + 7 );
	}

	// Do the same with string at last position.
	{
		MessageMeta end_meta;
		end_meta.add_field( MessageMeta::DWORD );
		end_meta.add_field( MessageMeta::DWORD );
		end_meta.add_field( MessageMeta::STRING );

		Message msg( end_meta );
		msg.set_dword( 0, 111 );
		msg.set_dword( 1, 333 );
		msg.set_string( 2, "Foo" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_dword( 1 ) == 333 );
		BOOST_CHECK( msg.get_string_length( 2 ) == 3 );
		BOOST_CHECK( msg.get_string( 2 ) == "Foo" );
		BOOST_CHECK( msg.get_size() == end_meta.get_minimum_size() + 3 );

		// Enlarge.
		msg.set_string( 2, "FooBar" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_dword( 1 ) == 333 );
		BOOST_CHECK( msg.get_string_length( 2 ) == 6 );
		BOOST_CHECK( msg.get_string( 2 ) == "FooBar" );
		BOOST_CHECK( msg.get_size() == end_meta.get_minimum_size() + 6 );

		// Enlarge again.
		msg.set_string( 2, "FooBarBaz" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_dword( 1 ) == 333 );
		BOOST_CHECK( msg.get_string_length( 2 ) == 9 );
		BOOST_CHECK( msg.get_string( 2 ) == "FooBarBaz" );
		BOOST_CHECK( msg.get_size() == end_meta.get_minimum_size() + 9 );

		// Shrink.
		msg.set_string( 2, "Bleh" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_dword( 1 ) == 333 );
		BOOST_CHECK( msg.get_string_length( 2 ) == 4 );
		BOOST_CHECK( msg.get_string( 2 ) == "Bleh" );
		BOOST_CHECK( msg.get_size() == end_meta.get_minimum_size() + 4 );

		// Shrink again.
		msg.set_string( 2, "" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_dword( 1 ) == 333 );
		BOOST_CHECK( msg.get_string_length( 2 ) == 0 );
		BOOST_CHECK( msg.get_string( 2 ) == "" );
		BOOST_CHECK( msg.get_size() == end_meta.get_minimum_size() );

		// Enlarge again.
		msg.set_string( 2, "DaFinal" );

		BOOST_CHECK( msg.get_dword( 0 ) == 111 );
		BOOST_CHECK( msg.get_dword( 1 ) == 333 );
		BOOST_CHECK( msg.get_string_length( 2 ) == 7 );
		BOOST_CHECK( msg.get_string( 2 ) == "DaFinal" );
		BOOST_CHECK( msg.get_size() == end_meta.get_minimum_size() + 7 );
	}
}
