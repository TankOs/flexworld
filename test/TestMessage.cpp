#include <boost/test/unit_test.hpp>

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/Login.hpp>
#include <FlexWorld/ServerProtocol.hpp>

BOOST_AUTO_TEST_CASE( TestMessage ) {
	using namespace flex;

	// Nothing to check (yet?).
}

BOOST_AUTO_TEST_CASE( TestLoginMessage ) {
	using namespace flex;

	const std::string USERNAME = "Tank";
	const std::string PASSWORD = "h4x0r";
	const std::size_t SIZE = 1 + USERNAME.size() + 1 + PASSWORD.size();

	// Initial state.
	{
		msg::Login msg;

		BOOST_CHECK( msg.get_username() == "" );
		BOOST_CHECK( msg.get_password() == "" );
	}

	// Basic properties.
	{
		msg::Login msg;
		msg.set_username( USERNAME );
		msg.set_password( PASSWORD );

		BOOST_CHECK( msg.get_username() == USERNAME );
		BOOST_CHECK( msg.get_password() == PASSWORD );
	}

	Protocol::Buffer source;
	source.push_back( static_cast<char>( USERNAME.size() ) );
	source.insert( source.end(), USERNAME.begin(), USERNAME.end() );
	source.push_back( static_cast<char>( PASSWORD.size() ) );
	source.insert( source.end(), PASSWORD.begin(), PASSWORD.end() );

	// Serialize.
	{
		msg::Login msg;
		msg.set_username( USERNAME );
		msg.set_password( PASSWORD );

		Protocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );
		BOOST_CHECK( buffer == source );
	}

	// Deserialize.
	{
		msg::Login msg;
		std::size_t result = 0;
		BOOST_CHECK_NO_THROW( result = msg.deserialize( source ) );
		BOOST_CHECK( result == SIZE );
		BOOST_CHECK( msg.get_username() == USERNAME );
		BOOST_CHECK( msg.get_password() == PASSWORD );
	}
}
