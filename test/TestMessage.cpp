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

	ServerProtocol::Buffer source;
	source.push_back( static_cast<char>( USERNAME.size() ) );
	source.insert( source.end(), USERNAME.begin(), USERNAME.end() );
	source.push_back( static_cast<char>( PASSWORD.size() ) );
	source.insert( source.end(), PASSWORD.begin(), PASSWORD.end() );

	// Serialize.
	{
		msg::Login msg;
		msg.set_username( USERNAME );
		msg.set_password( PASSWORD );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );
		BOOST_CHECK( buffer == source );
	}

	// Serialize wrong data.
	{
		msg::Login msg;
		ServerProtocol::Buffer buffer;

		// No props.
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Login::InvalidDataException );

		// Too long username.
		msg.set_username( "123456789012345678901234X" );
		msg.set_password( "okay" );
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Login::InvalidDataException );

		// Password only.
		msg.set_username( "" );
		msg.set_password( "okay" );
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Login::InvalidDataException );
	}

	// Deserialize.
	{
		msg::Login msg;
		std::size_t result = 0;
		BOOST_CHECK_NO_THROW( result = msg.deserialize( &source[0], source.size() ) );
		BOOST_CHECK( result == SIZE );
		BOOST_CHECK( msg.get_username() == USERNAME );
		BOOST_CHECK( msg.get_password() == PASSWORD );
	}

	// Deserialize wrong data.
	{
		// Zero-length username.
		{
			ServerProtocol::Buffer buffer( 1, 0 );
			msg::Login msg;
			BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::Login::BogusDataException );
		}

		// Too long username.
		{
			ServerProtocol::Buffer buffer( 1, msg::Login::MAX_USERNAME_LENGTH + 1 );
			msg::Login msg;
			BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::Login::BogusDataException );
		}

		// Zero-length password.
		{
			ServerProtocol::Buffer buffer( 3, 0 );
			buffer[0] = 0x01;
			buffer[1] = 'a';

			msg::Login msg;
			BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::Login::BogusDataException );
		}
	}
}
