#include "TestServerProtocolReactor.hpp"

#include <FlexWorld/ServerProtocol.hpp>
#include <FlexWorld/Socket.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestServerProtocol ) {
	using namespace flex;

	TestServerProtocolReactor reactor;
	Protocol::Buffer buffer;

	// Login message.
	{
		ServerProtocol::build_login_message( "Tank", "foo", buffer );
		const std::size_t SIZE = 12;

		BOOST_REQUIRE( buffer.size() == SIZE );

		std::size_t index( 0 );
		BOOST_CHECK( buffer[index++] == 0x01 );
		BOOST_CHECK( buffer[index++] == 0x04 );
		BOOST_CHECK( buffer[index++] == 0x00 );
		BOOST_CHECK( buffer[index++] == 'T' );
		BOOST_CHECK( buffer[index++] == 'a' );
		BOOST_CHECK( buffer[index++] == 'n' );
		BOOST_CHECK( buffer[index++] == 'k' );
		BOOST_CHECK( buffer[index++] == 0x03 );
		BOOST_CHECK( buffer[index++] == 0x00 );
		BOOST_CHECK( buffer[index++] == 'f' );
		BOOST_CHECK( buffer[index++] == 'o' );
		BOOST_CHECK( buffer[index++] == 'o' );

		BOOST_CHECK( reactor.handle_incoming_data( 1337, buffer ) == SIZE );
		BOOST_CHECK( reactor.m_sender == 1337 );
		BOOST_CHECK( reactor.m_login_username == "Tank" );
		BOOST_CHECK( reactor.m_login_password == "foo" );
	}
}
