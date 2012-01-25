#include <boost/test/unit_test.hpp>

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/Beam.hpp>
#include <FlexWorld/Messages/Chunk.hpp>
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

BOOST_AUTO_TEST_CASE( TestReadyMessage ) {
	using namespace flex;

	const std::size_t SIZE = sizeof( uint8_t );

	// Initial state.
	{
		msg::Ready msg;
	}

	// Basic properties.
	{
		msg::Ready msg;
	}

	ServerProtocol::Buffer source( 1, 0 );

	// Serialize.
	{
		msg::Ready msg;

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Deserialize.
	{
		msg::Ready msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );
		BOOST_CHECK( eaten == SIZE );
	}

	// Deserialize with too less data.
	{
		msg::Ready msg;

		for( std::size_t amount = 0; amount < SIZE; ++amount ) {
			ServerProtocol::Buffer buffer( amount, 0 );
			BOOST_CHECK( msg.deserialize( &buffer[0], buffer.size() ) == 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( TestBeamMessage ) {
	using namespace flex;

	const std::string PLANET_NAME = "construct";
	const sf::Vector3f POSITION = sf::Vector3f( 1, 2, 3 );
	const uint16_t ANGLE = 180;
	const Planet::Vector PLANET_SIZE = Planet::Vector( 10, 20, 30 );
	const Chunk::Vector CHUNK_SIZE = Chunk::Vector( 32, 64, 128 );

	const std::size_t SIZE =
		+ sizeof( uint8_t ) // Planet name length.
		+ sizeof( char ) * PLANET_NAME.size()
		+ sizeof( POSITION )
		+ sizeof( ANGLE )
		+ sizeof( PLANET_SIZE )
		+ sizeof( CHUNK_SIZE )
	;

	// Initial state.
	{
		msg::Beam msg;

		BOOST_CHECK( msg.get_planet_name() == "" );
		BOOST_CHECK( msg.get_position() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( msg.get_angle() == 0 );
		BOOST_CHECK( msg.get_planet_size() == Planet::Vector( 0, 0, 0 ) );
		BOOST_CHECK( msg.get_chunk_size() == Chunk::Vector( 0, 0, 0 ) );
	}

	// Basic properties.
	{
		msg::Beam msg;

		msg.set_planet_name( PLANET_NAME );
		msg.set_position( POSITION );
		msg.set_angle( ANGLE );
		msg.set_planet_size( PLANET_SIZE );
		msg.set_chunk_size( CHUNK_SIZE );

		BOOST_CHECK( msg.get_planet_name() == PLANET_NAME );
		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_angle() == ANGLE );
		BOOST_CHECK( msg.get_planet_size() == PLANET_SIZE );
		BOOST_CHECK( msg.get_chunk_size() == CHUNK_SIZE );
	}

	ServerProtocol::Buffer source;
	source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
	source.insert( source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
	source.insert( source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

	// Serialize.
	{
		msg::Beam msg;

		msg.set_planet_name( PLANET_NAME );
		msg.set_position( POSITION );
		msg.set_angle( ANGLE );
		msg.set_planet_size( PLANET_SIZE );
		msg.set_chunk_size( CHUNK_SIZE );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Serialize with invalid planet name.
	{
		msg::Beam msg;
		msg.set_planet_name( "" );
		msg.set_position( POSITION );
		msg.set_angle( ANGLE );
		msg.set_planet_size( PLANET_SIZE );
		msg.set_chunk_size( CHUNK_SIZE );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );

		std::vector<char> invalid_name( msg::Beam::MAX_PLANET_NAME_LENGTH + 1, 'a' );
		msg.set_planet_name( std::string( &invalid_name.front(), invalid_name.size() ) );
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
	}

	// Serialize with invalid position.
	{
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( sf::Vector3f( -1, 0, 0 ) );
			msg.set_angle( ANGLE );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( sf::Vector3f( 0, -1, 0 ) );
			msg.set_angle( ANGLE );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( sf::Vector3f( 0, 0, -1 ) );
			msg.set_angle( ANGLE );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
	}

	// Serialize with invalid planet size.
	{
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_angle( ANGLE );
			msg.set_planet_size( Planet::Vector( 1, 1, 0 ) );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_angle( ANGLE );
			msg.set_planet_size( Planet::Vector( 1, 0, 1 ) );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_angle( ANGLE );
			msg.set_planet_size( Planet::Vector( 0, 1, 1 ) );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
	}

	// Serialize with invalid chunk size.
	{
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_angle( ANGLE );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( Chunk::Vector( 0, 1, 1 ) );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_angle( ANGLE );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( Chunk::Vector( 1, 0, 1 ) );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_angle( ANGLE );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( Chunk::Vector( 1, 1, 0 ) );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );
		}
	}

	// Deserialize.
	{
		msg::Beam msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );

		BOOST_CHECK( eaten == SIZE );
		BOOST_CHECK( msg.get_planet_name() == PLANET_NAME );
		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_angle() == ANGLE );
		BOOST_CHECK( msg.get_planet_size() == PLANET_SIZE );
		BOOST_CHECK( msg.get_chunk_size() == CHUNK_SIZE );
	}

	// Deserialize with invalid planet name.
	{
		ServerProtocol::Buffer i_buffer( 1, 0 );

		msg::Beam msg;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = msg.deserialize( &i_buffer.front(), i_buffer.size() ), msg::Beam::BogusDataException );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with invalid position.
	{
		{
			sf::Vector3f invalid_position( -1, 0, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_position ), reinterpret_cast<const char*>( &invalid_position ) + sizeof( invalid_position ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
		{
			sf::Vector3f invalid_position( 0, -1, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_position ), reinterpret_cast<const char*>( &invalid_position ) + sizeof( invalid_position ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
		{
			sf::Vector3f invalid_position( 0, 0, -1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_position ), reinterpret_cast<const char*>( &invalid_position ) + sizeof( invalid_position ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
	}

	// Deserialize with invalid planet size.
	{
		{
			Planet::Vector invalid_size( 1, 0, 1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Planet::Vector invalid_size( 1, 1, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Planet::Vector invalid_size( 0, 1, 1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
	}

	// Deserialize with invalid chunk size.
	{
		{
			Chunk::Vector invalid_size( 1, 0, 1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Chunk::Vector invalid_size( 1, 1, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Chunk::Vector invalid_size( 0, 1, 1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ANGLE ), reinterpret_cast<const char*>( &ANGLE ) + sizeof( ANGLE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException );
			BOOST_CHECK( eaten == 0 );
		}
	}

	// Deserialize with too less data.
	{
		msg::Beam msg;

		for( std::size_t amount = 0; amount < source.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source[0], amount ) == 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( TestChunkMessage ) {
	using namespace flex;

	// Initial state.
	{
		msg::Chunk msg;

		BOOST_CHECK( msg.get_num_blocks() == 0 );
	}

	enum { CHUNK_SIZE = 16 };

	std::vector<std::string> class_ids;
	class_ids.push_back( "fw.base.nature/grass" );
	class_ids.push_back( "fw.base.human/dwarf_male" );
	class_ids.push_back( "fw.base.primitives/cube" );

	// Add blocks.
	{
		msg::Chunk msg;

		std::size_t class_id_idx = 0;
		uint8_t flags = 0;

		for( std::size_t block_idx = 0; block_idx < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++block_idx ) {
			msg.add_block( class_ids[class_id_idx], flags );

			class_id_idx = (class_id_idx + 1) % class_ids.size();
			flags = (flags + 1) & 0x0f;
		}

		BOOST_CHECK( msg.get_num_blocks() == CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE );

		class_id_idx = 0;
		flags = 0;

		for( std::size_t block_idx = 0; block_idx < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++block_idx ) {
			BOOST_CHECK( msg.get_block_class_id( block_idx ) == class_ids[class_id_idx] );
			BOOST_CHECK( msg.get_block_flags( block_idx ) == flags );

			class_id_idx = (class_id_idx + 1) % class_ids.size();
			flags = (flags + 1) & 0x0f;
		}
	}

	static const uint16_t NUM_CLASS_IDS = static_cast<uint16_t>( class_ids.size() );

	ServerProtocol::Buffer source_buffer;

	// Add class IDs to buffer.
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &NUM_CLASS_IDS ), reinterpret_cast<const char*>( &NUM_CLASS_IDS ) + sizeof( NUM_CLASS_IDS ) );

	for( std::size_t class_id_idx = 0; class_id_idx < class_ids.size(); ++class_id_idx ) {
		std::uint8_t class_id_length = static_cast<uint8_t>( class_ids[class_id_idx].size() );

		source_buffer.push_back( class_id_length );
		source_buffer.insert( source_buffer.end(), class_ids[class_id_idx].c_str(), class_ids[class_id_idx].c_str() + class_id_length );
	}

	// Add blocks to buffer.
	uint8_t flags = 0;
	uint16_t class_id_idx = 0;

	// Number of blocks.
	uint16_t num_blocks = static_cast<uint16_t>( CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &num_blocks ), reinterpret_cast<const char*>( &num_blocks ) + sizeof( num_blocks ) );

	for( std::size_t block_idx = 0; block_idx < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++block_idx ) {
		uint16_t ref = static_cast<uint16_t>( (flags << 12) | (class_id_idx & 0xfff) );
		source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &ref ), reinterpret_cast<const char*>( &ref ) + sizeof( ref ) );

		class_id_idx = static_cast<uint16_t>( (class_id_idx + 1) % class_ids.size() );
		flags = (flags + 1) & 0x0f;
	}

	// Serialize.
	{
		msg::Chunk msg;

		class_id_idx = 0;
		flags = 0;

		for( std::size_t block_idx = 0; block_idx < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++block_idx ) {
			msg.add_block( class_ids[class_id_idx], flags );

			class_id_idx = static_cast<uint16_t>( (class_id_idx + 1) % class_ids.size() );
			flags = (flags + 1) & 0x0f;
		}

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );
		BOOST_CHECK( buffer == source_buffer );
	}

	// Serialize without data.
	{
		msg::Chunk msg;

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Chunk::InvalidDataException );
	}

	// Deserialize.
	{
		msg::Chunk msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source_buffer[0], source_buffer.size() ) );

		BOOST_CHECK( eaten == source_buffer.size() );
	}

	// Deserialize without class IDs.
	{
		ServerProtocol::Buffer invalid_buffer( 2, 0 );

		msg::Chunk msg;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_buffer[0], invalid_buffer.size() ), msg::Chunk::BogusDataException );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with too many class IDs.
	{
		ServerProtocol::Buffer invalid_buffer( 2, 0 );
		invalid_buffer[0] = static_cast<uint8_t>( 0xff );
		invalid_buffer[1] = static_cast<uint8_t>( 0x10 );

		msg::Chunk msg;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_buffer[0], invalid_buffer.size() ), msg::Chunk::BogusDataException );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with zero class ID length.
	{
		ServerProtocol::Buffer invalid_buffer( 3, 0 );
		invalid_buffer[0] = static_cast<uint8_t>( 0x01 );

		msg::Chunk msg;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_buffer[0], invalid_buffer.size() ), msg::Chunk::BogusDataException );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with invalid number of blocks.
	{
		ServerProtocol::Buffer invalid_buffer( 6 );
		invalid_buffer[0] = static_cast<uint8_t>( 0x01 ); // Num class IDs.
		invalid_buffer[1] = static_cast<uint8_t>( 0x00 );

		invalid_buffer[2] = static_cast<uint8_t>( 0x01 ); // Class ID: 1 char.
		invalid_buffer[3] = 'X';

		invalid_buffer[4] = static_cast<uint8_t>( 0x00 ); // Number of blocks.
		invalid_buffer[5] = static_cast<uint8_t>( 0x00 );

		msg::Chunk msg;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_buffer[0], invalid_buffer.size() ), msg::Chunk::BogusDataException );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with invalid referenced class ID.
	{
		ServerProtocol::Buffer invalid_buffer( 8 );
		invalid_buffer[0] = static_cast<uint8_t>( 0x01 ); // Num class IDs.
		invalid_buffer[1] = static_cast<uint8_t>( 0x00 );

		invalid_buffer[2] = static_cast<uint8_t>( 0x01 ); // Class ID: 1 char.
		invalid_buffer[3] = 'X';

		invalid_buffer[4] = static_cast<uint8_t>( 0x01 ); // Number of blocks.
		invalid_buffer[5] = static_cast<uint8_t>( 0x00 );

		invalid_buffer[6] = static_cast<uint8_t>( 0x01 ); // Block.
		invalid_buffer[7] = static_cast<uint8_t>( 0x00 );

		msg::Chunk msg;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = msg.deserialize( &invalid_buffer[0], invalid_buffer.size() ), msg::Chunk::BogusDataException );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with too less data.
	{
		msg::Chunk msg;

		for( std::size_t amount = 0; amount < source_buffer.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source_buffer[0], amount ) == 0 );
		}
	}
}
