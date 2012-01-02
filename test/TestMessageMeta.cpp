#include <boost/test/unit_test.hpp>

#include <FlexWorld/MessageMeta.hpp>

BOOST_AUTO_TEST_CASE( TestMessageMeta ) {
	using namespace flex;

	// Initial state.
	{
		MessageMeta meta;

		BOOST_CHECK( meta.get_num_fields() == 0 );
		BOOST_CHECK( meta.get_minimum_size() == 0 );
	}

	// Check field sizes.
	{
		BOOST_CHECK( MessageMeta::get_field_size( MessageMeta::BYTE ) == sizeof( MessageMeta::ByteType ) );
		BOOST_CHECK( MessageMeta::get_field_size( MessageMeta::WORD ) == sizeof( MessageMeta::WordType ) );
		BOOST_CHECK( MessageMeta::get_field_size( MessageMeta::DWORD ) == sizeof( MessageMeta::DWordType ) );
		BOOST_CHECK( MessageMeta::get_field_size( MessageMeta::STRING ) == sizeof( MessageMeta::StringLengthType ) );
	}

	// Add fields.
	{
		MessageMeta meta;

		meta.add_field( MessageMeta::STRING );
		meta.add_field( MessageMeta::DWORD );
		meta.add_field( MessageMeta::WORD );
		meta.add_field( MessageMeta::BYTE );

		BOOST_CHECK( meta.get_field( 0 ) == MessageMeta::STRING );
		BOOST_CHECK( meta.get_field( 1 ) == MessageMeta::DWORD );
		BOOST_CHECK( meta.get_field( 2 ) == MessageMeta::WORD );
		BOOST_CHECK( meta.get_field( 3 ) == MessageMeta::BYTE );

		BOOST_CHECK(
			meta.get_minimum_size() == (
				sizeof( MessageMeta::StringLengthType ) +
				sizeof( MessageMeta::DWordType ) +
				sizeof( MessageMeta::WordType ) +
				sizeof( MessageMeta::ByteType )
			)
		);
	}
}
