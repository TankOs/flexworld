#include <boost/test/unit_test.hpp>

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/ServerProtocol.hpp>

BOOST_AUTO_TEST_CASE( TestMessage ) {
	using namespace flex;

	// Nothing to check (yet?).
}

BOOST_AUTO_TEST_CASE( TestOpenLoginMessage ) {
	using namespace flex;

	const std::string USERNAME = "Tank";
	const std::string PASSWORD = "h4x0r";
	const std::string SERVER_PASSWORD = "s3rv3r";
	const std::size_t SIZE = 1 + USERNAME.size() + 1 + PASSWORD.size() + 1 + SERVER_PASSWORD.size();

	// Initial state.
	{
		msg::OpenLogin msg;

		BOOST_CHECK( msg.get_username() == "" );
		BOOST_CHECK( msg.get_password() == "" );
		BOOST_CHECK( msg.get_server_password() == "" );
	}

	// Basic properties.
	{
		msg::OpenLogin msg;
		msg.set_username( USERNAME );
		msg.set_password( PASSWORD );
		msg.set_server_password( SERVER_PASSWORD );

		BOOST_CHECK( msg.get_username() == USERNAME );
		BOOST_CHECK( msg.get_password() == PASSWORD );
	}

	ServerProtocol::Buffer source;
	source.push_back( static_cast<char>( USERNAME.size() ) );
	source.insert( source.end(), USERNAME.begin(), USERNAME.end() );
	source.push_back( static_cast<char>( PASSWORD.size() ) );
	source.insert( source.end(), PASSWORD.begin(), PASSWORD.end() );
	source.push_back( static_cast<char>( SERVER_PASSWORD.size() ) );
	source.insert( source.end(), SERVER_PASSWORD.begin(), SERVER_PASSWORD.end() );

	// Serialize.
	{
		msg::OpenLogin msg;
		msg.set_username( USERNAME );
		msg.set_password( PASSWORD );
		msg.set_server_password( SERVER_PASSWORD );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );
		BOOST_CHECK( buffer == source );
	}

	// Serialize wrong data.
	{
		msg::OpenLogin msg;
		ServerProtocol::Buffer buffer;

		// No props.
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::OpenLogin::InvalidDataException );

		// Too long username.
		msg.set_username( "123456789012345678901234X" );
		msg.set_password( "okay" );
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::OpenLogin::InvalidDataException );

		// Password only.
		msg.set_username( "" );
		msg.set_password( "okay" );
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::OpenLogin::InvalidDataException );
	}

	// Deserialize.
	{
		msg::OpenLogin msg;
		std::size_t result = 0;
		BOOST_CHECK_NO_THROW( result = msg.deserialize( &source[0], source.size() ) );
		BOOST_CHECK( result == SIZE );
		BOOST_CHECK( msg.get_username() == USERNAME );
		BOOST_CHECK( msg.get_password() == PASSWORD );
		BOOST_CHECK( msg.get_server_password() == SERVER_PASSWORD );
	}

	// Deserialize wrong data.
	{
		// Zero-length username.
		{
			ServerProtocol::Buffer buffer( 1, 0 );
			msg::OpenLogin msg;
			BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::OpenLogin::BogusDataException );
		}

		// Too long username.
		{
			ServerProtocol::Buffer buffer( 1, msg::OpenLogin::MAX_USERNAME_LENGTH + 1 );
			msg::OpenLogin msg;
			BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::OpenLogin::BogusDataException );
		}

		// Zero-length password.
		{
			ServerProtocol::Buffer buffer( 3, 0 );
			buffer[0] = 0x01;
			buffer[1] = 'a';

			msg::OpenLogin msg;
			BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::OpenLogin::BogusDataException );
		}
	}

	// Check that deserializing wrong data doesn't modify the message's properties.
	{
		msg::OpenLogin msg;
		msg.set_username( USERNAME );
		msg.set_password( PASSWORD );
		msg.set_server_password( SERVER_PASSWORD );

		ServerProtocol::Buffer buffer( 1, 0 );
		BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::OpenLogin::BogusDataException );

		BOOST_CHECK( msg.get_username() == USERNAME );
		BOOST_CHECK( msg.get_password() == PASSWORD );
		BOOST_CHECK( msg.get_server_password() == SERVER_PASSWORD );
	}
}

BOOST_AUTO_TEST_CASE( TestServerInfoMessage ) {
	using namespace flex;

	const msg::ServerInfo::AuthMode AUTH_MODE = msg::ServerInfo::KEY_AUTH;
	const uint8_t FLAGS = msg::ServerInfo::PASSWORD_FLAG;
	const std::size_t SIZE = sizeof( uint8_t ) + sizeof( uint8_t );

	// Initial state.
	{
		msg::ServerInfo msg;

		BOOST_CHECK( msg.get_auth_mode() == msg::ServerInfo::OPEN_AUTH );
		BOOST_CHECK( msg.get_flags() == msg::ServerInfo::NO_FLAGS );
	}

	// Basic properties.
	{
		msg::ServerInfo msg;
		msg.set_auth_mode( AUTH_MODE );
		msg.set_flags( FLAGS );

		BOOST_CHECK( msg.get_auth_mode() == AUTH_MODE );
		BOOST_CHECK( msg.get_flags() == FLAGS );
	}

	ServerProtocol::Buffer source;
	source.push_back( static_cast<char>( AUTH_MODE ) );
	source.push_back( static_cast<char>( FLAGS ) );

	// Serialize.
	{
		msg::ServerInfo msg;
		msg.set_auth_mode( AUTH_MODE );
		msg.set_flags( FLAGS );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Deserialize.
	{
		msg::ServerInfo msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );
		BOOST_CHECK( eaten == SIZE );

		BOOST_CHECK( msg.get_auth_mode() == AUTH_MODE );
		BOOST_CHECK( msg.get_flags() == FLAGS );
	}

	// Deserialize with too less data.
	{
		msg::ServerInfo msg;

		for( std::size_t amount = 0; amount < SIZE; ++amount ) {
			ServerProtocol::Buffer buffer( amount, 0 );
			BOOST_CHECK( msg.deserialize( &buffer[0], buffer.size() ) == 0 );
		}
	}

	// Deserialize wrong auth mode.
	{
		ServerProtocol::Buffer buffer( SIZE, 5 );
		msg::ServerInfo msg;

		BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::ServerInfo::BogusDataException );
	}

	// Check deserializing wrong data doesn't change previous properties.
	{
		msg::ServerInfo msg;
		msg.set_auth_mode( AUTH_MODE );
		msg.set_flags( FLAGS );

		ServerProtocol::Buffer buffer( SIZE, 5 );
		BOOST_CHECK_THROW( msg.deserialize( &buffer[0], buffer.size() ), msg::ServerInfo::BogusDataException );

		BOOST_CHECK( msg.get_auth_mode() == AUTH_MODE );
		BOOST_CHECK( msg.get_flags() == FLAGS );
	}
}

BOOST_AUTO_TEST_CASE( TestLoginOKMessage ) {
	using namespace flex;

	const std::size_t SIZE = sizeof( uint8_t );

	// Initial state.
	{
		msg::LoginOK msg;
	}

	// Basic properties.
	{
		msg::LoginOK msg;
	}

	ServerProtocol::Buffer source( 1, 0 );

	// Serialize.
	{
		msg::LoginOK msg;

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Deserialize.
	{
		msg::LoginOK msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );
		BOOST_CHECK( eaten == SIZE );
	}

	// Deserialize with too less data.
	{
		msg::LoginOK msg;

		for( std::size_t amount = 0; amount < SIZE; ++amount ) {
			ServerProtocol::Buffer buffer( amount, 0 );
			BOOST_CHECK( msg.deserialize( &buffer[0], buffer.size() ) == 0 );
		}
	}
}
