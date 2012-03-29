#include <boost/test/unit_test.hpp>

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/Beam.hpp>
#include <FlexWorld/Messages/Chunk.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Messages/ChunkUnchanged.hpp>
#include <FlexWorld/Messages/CreateEntity.hpp>
#include <FlexWorld/Messages/Chat.hpp>
#include <FlexWorld/ServerProtocol.hpp>
#include <iostream>

template <class T>
struct ExceptionChecker {
	ExceptionChecker( const std::string& msg_ ) :
		msg( msg_ )
	{
	}

	bool operator()( const T& e ) {
		bool result = (e.what() == msg);

		if( !result ) {
			std::cout << "Expected: " << msg << std::endl;
			std::cout << "Thrown:   " << e.what() << std::endl;
		}

		return result;
	}

	std::string msg;
};

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

	const std::size_t SIZE = sizeof( Entity::ID );

	// Initial state.
	{
		msg::LoginOK msg;

		BOOST_CHECK( msg.get_entity_id() == 0 );
	}

	// Basic properties.
	{
		msg::LoginOK msg;

		msg.set_entity_id( 123 );

		BOOST_CHECK( msg.get_entity_id() == 123 );
	}

	static const Entity::ID ENTITY_ID = 123;

	ServerProtocol::Buffer source( SIZE, 0 );
	source[0] = ENTITY_ID;

	// Serialize.
	{
		msg::LoginOK msg;

		msg.set_entity_id( ENTITY_ID );

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

		BOOST_CHECK( msg.get_entity_id() == ENTITY_ID );
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
	const float HEADING = 233.124f;
	const Planet::Vector PLANET_SIZE = Planet::Vector( 10, 20, 30 );
	const Chunk::Vector CHUNK_SIZE = Chunk::Vector( 32, 64, 128 );

	const std::size_t SIZE =
		+ sizeof( uint8_t ) // Planet name length.
		+ sizeof( char ) * PLANET_NAME.size()
		+ sizeof( POSITION )
		+ sizeof( HEADING )
		+ sizeof( PLANET_SIZE )
		+ sizeof( CHUNK_SIZE )
	;

	// Initial state.
	{
		msg::Beam msg;

		BOOST_CHECK( msg.get_planet_name() == "" );
		BOOST_CHECK( msg.get_position() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( msg.get_heading() == 0 );
		BOOST_CHECK( msg.get_planet_size() == Planet::Vector( 0, 0, 0 ) );
		BOOST_CHECK( msg.get_chunk_size() == Chunk::Vector( 0, 0, 0 ) );
	}

	// Basic properties.
	{
		msg::Beam msg;

		msg.set_planet_name( PLANET_NAME );
		msg.set_position( POSITION );
		msg.set_heading( HEADING );
		msg.set_planet_size( PLANET_SIZE );
		msg.set_chunk_size( CHUNK_SIZE );

		BOOST_CHECK( msg.get_planet_name() == PLANET_NAME );
		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_heading() == HEADING );
		BOOST_CHECK( msg.get_planet_size() == PLANET_SIZE );
		BOOST_CHECK( msg.get_chunk_size() == CHUNK_SIZE );
	}

	ServerProtocol::Buffer source;
	source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
	source.insert( source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
	source.insert( source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

	// Serialize.
	{
		msg::Beam msg;

		msg.set_planet_name( PLANET_NAME );
		msg.set_position( POSITION );
		msg.set_heading( HEADING );
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
		msg.set_heading( HEADING );
		msg.set_planet_size( PLANET_SIZE );
		msg.set_chunk_size( CHUNK_SIZE );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_THROW( msg.serialize( buffer ), msg::Beam::InvalidDataException );

		std::vector<char> invalid_name( msg::Beam::MAX_PLANET_NAME_LENGTH + 1, 'a' );
		msg.set_planet_name( std::string( &invalid_name.front(), invalid_name.size() ) );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid planet name." ) );
	}

	// Serialize with invalid position.
	{
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( sf::Vector3f( -1, 0, 0 ) );
			msg.set_heading( HEADING );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid position." ) );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( sf::Vector3f( 0, -1, 0 ) );
			msg.set_heading( HEADING );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid position." ) );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( sf::Vector3f( 0, 0, -1 ) );
			msg.set_heading( HEADING );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid position." ) );
		}
	}

	// Serialize with invalid planet size.
	{
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_heading( HEADING );
			msg.set_planet_size( Planet::Vector( 1, 1, 0 ) );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid planet size." ) );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_heading( HEADING );
			msg.set_planet_size( Planet::Vector( 1, 0, 1 ) );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid planet size." ) );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_heading( HEADING );
			msg.set_planet_size( Planet::Vector( 0, 1, 1 ) );
			msg.set_chunk_size( CHUNK_SIZE );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid planet size." ) );
		}
	}

	// Serialize with invalid chunk size.
	{
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_heading( HEADING );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( Chunk::Vector( 0, 1, 1 ) );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid chunk size." ) );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_heading( HEADING );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( Chunk::Vector( 1, 0, 1 ) );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid chunk size." ) );
		}
		{
			msg::Beam msg;
			msg.set_planet_name( PLANET_NAME );
			msg.set_position( POSITION );
			msg.set_heading( HEADING );
			msg.set_planet_size( PLANET_SIZE );
			msg.set_chunk_size( Chunk::Vector( 1, 1, 0 ) );

			ServerProtocol::Buffer buffer;
			BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Beam::InvalidDataException, ExceptionChecker<msg::Beam::InvalidDataException>( "Invalid chunk size." ) );
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
		BOOST_CHECK( msg.get_heading() == HEADING );
		BOOST_CHECK( msg.get_planet_size() == PLANET_SIZE );
		BOOST_CHECK( msg.get_chunk_size() == CHUNK_SIZE );
	}

	// Deserialize with invalid planet name.
	{
		ServerProtocol::Buffer i_buffer( 1, 0 );

		msg::Beam msg;
		std::size_t eaten = 0;

		BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &i_buffer.front(), i_buffer.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid planet name." ) );
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
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid position." ) );
			BOOST_CHECK( eaten == 0 );
		}
		{
			sf::Vector3f invalid_position( 0, -1, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_position ), reinterpret_cast<const char*>( &invalid_position ) + sizeof( invalid_position ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid position." ) );
			BOOST_CHECK( eaten == 0 );
		}
		{
			sf::Vector3f invalid_position( 0, 0, -1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_position ), reinterpret_cast<const char*>( &invalid_position ) + sizeof( invalid_position ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid position." ) );
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
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid planet size." ) );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Planet::Vector invalid_size( 1, 1, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid planet size." ) );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Planet::Vector invalid_size( 0, 1, 1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &CHUNK_SIZE ), reinterpret_cast<const char*>( &CHUNK_SIZE ) + sizeof( CHUNK_SIZE ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid planet size." ) );
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
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid chunk size." ) );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Chunk::Vector invalid_size( 1, 1, 0 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid chunk size." ) );
			BOOST_CHECK( eaten == 0 );
		}
		{
			Chunk::Vector invalid_size( 0, 1, 1 );

			ServerProtocol::Buffer invalid_source;
			invalid_source.push_back( static_cast<unsigned char>( PLANET_NAME.size() ) );
			invalid_source.insert( invalid_source.end(), PLANET_NAME.begin(), PLANET_NAME.end() );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &PLANET_SIZE ), reinterpret_cast<const char*>( &PLANET_SIZE ) + sizeof( PLANET_SIZE ) );
			invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &invalid_size ), reinterpret_cast<const char*>( &invalid_size ) + sizeof( invalid_size ) );

			msg::Beam msg;
			std::size_t eaten = 0;

			BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid chunk size." ) );
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

	enum { CHUNK_SIZE = 16 };
	static const Planet::Vector POSITION( 22, 33, 44 );

	// Initial state.
	{
		msg::Chunk msg;

		BOOST_CHECK( msg.get_num_blocks() == 0 );
		BOOST_CHECK( msg.get_position() == Planet::Vector( 0, 0, 0 ) );
	}

	// Basic properties.
	{
		msg::Chunk msg;

		msg.set_position( POSITION );

		BOOST_CHECK( msg.get_position() == POSITION );
	}

	// Create chunk for testing.
	Chunk source_chunk( Chunk::Vector( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE ) );

	{
		Chunk::Vector runner( 0, 0, 0 );
		Chunk::Block block = 0;

		for( runner.z = 0; runner.z < CHUNK_SIZE; ++runner.z ) {
			for( runner.y = 0; runner.y < CHUNK_SIZE; ++runner.y ) {
				for( runner.x = 0; runner.x < CHUNK_SIZE; ++runner.x ) {
					source_chunk.set_block( runner, block );
					block = static_cast<Chunk::Block>( (block + 1) % (Chunk::MAX_BLOCK_ID + 1) );
				}
			}
		}
	}

	// Set block data.
	{
		msg::Chunk msg;
		msg.set_blocks( source_chunk );

		BOOST_CHECK( msg.get_num_blocks() == CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE );

		// Validate.
		Chunk::Vector runner( 0, 0, 0 );
		Chunk::Block block = 0;
		bool all_sane( true );

		for( std::size_t block_idx = 0; block_idx < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++block_idx ) {
			if( msg.get_block( block_idx ) != block ) {
				all_sane = false;
			}

			block = static_cast<Chunk::Block>( (block + 1) % (Chunk::MAX_BLOCK_ID + 1) );
		}

		BOOST_CHECK( all_sane == true );
	}

	ServerProtocol::Buffer source;
	source.insert( source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );

	uint16_t num_blocks = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	source.insert( source.end(), reinterpret_cast<const char*>( &num_blocks ), reinterpret_cast<const char*>( &num_blocks ) + sizeof( num_blocks ) );
	source.insert( source.end(), reinterpret_cast<const char*>( source_chunk.get_raw_data() ), reinterpret_cast<const char*>( source_chunk.get_raw_data() ) + sizeof( Chunk::Block ) * num_blocks );

	// Serialize.
	{
		msg::Chunk msg;

		msg.set_position( POSITION );
		msg.set_blocks( source_chunk );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( source == buffer );
	}

	// Serialize with zero block count.
	{
		msg::Chunk msg;

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chunk::InvalidDataException, ExceptionChecker<msg::Chunk::InvalidDataException>( "Missing block data." ) );
	}

	// Serialize with too many blocks.
	{
		Chunk big_chunk( Chunk::Vector( 64, 64, 64 ) );

		msg::Chunk msg;

		msg.set_blocks( big_chunk );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chunk::InvalidDataException, ExceptionChecker<msg::Chunk::InvalidDataException>( "Too many blocks." ) );
	}

	// Deserialize with too less data.
	{
		msg::Chunk msg;

		for( std::size_t amount = 0; amount < source.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source[0], amount ) == 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( TestRequestChunkMessage ) {
	using namespace flex;

	static const Planet::Vector POSITION( 1, 2, 3 );
	static const msg::RequestChunk::Timestamp TIMESTAMP( 12345 );

	// Create source buffer.
	ServerProtocol::Buffer source;
	source.insert( source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &TIMESTAMP ), reinterpret_cast<const char*>( &TIMESTAMP ) + sizeof( TIMESTAMP ) );

	// Initial state.
	{
		msg::RequestChunk msg;

		BOOST_CHECK( msg.get_position() == Planet::Vector( 0, 0, 0 ) );
		BOOST_CHECK( msg.get_timestamp() == 0 );
	}

	// Basic properties.
	{
		msg::RequestChunk msg;
		msg.set_position( POSITION );
		msg.set_timestamp( TIMESTAMP );

		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_timestamp() == TIMESTAMP );
	}

	// Serialize.
	{
		msg::RequestChunk msg;
		msg.set_position( POSITION );
		msg.set_timestamp( TIMESTAMP );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Deserialize.
	{
		msg::RequestChunk msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );

		BOOST_CHECK( eaten == source.size() );
		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_timestamp() == TIMESTAMP );
	}

	// Deserialize with too less data.
	{
		msg::RequestChunk msg;

		for( std::size_t amount = 0; amount < source.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source[0], amount ) == 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( TestChunkUnchangedMessage ) {
	using namespace flex;

	static const Planet::Vector POSITION( 1, 2, 3 );

	// Create source buffer.
	ServerProtocol::Buffer source;
	source.insert( source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );

	// Initial state.
	{
		msg::ChunkUnchanged msg;

		BOOST_CHECK( msg.get_position() == Planet::Vector( 0, 0, 0 ) );
	}

	// Basic properties.
	{
		msg::ChunkUnchanged msg;
		msg.set_position( POSITION );

		BOOST_CHECK( msg.get_position() == POSITION );
	}

	// Serialize.
	{
		msg::ChunkUnchanged msg;
		msg.set_position( POSITION );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Deserialize.
	{
		msg::ChunkUnchanged msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );

		BOOST_CHECK( eaten == source.size() );
		BOOST_CHECK( msg.get_position() == POSITION );
	}

	// Deserialize with too less data.
	{
		msg::ChunkUnchanged msg;

		for( std::size_t amount = 0; amount < source.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source[0], amount ) == 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( TestCreateEntityMessage ) {
	using namespace flex;

	static const Entity::ID ID = 1337;
	static const Planet::Coordinate POSITION( 1, 2, 3 );
	static const std::string CLASS( "fw.base.human/dwarf_male" );
	static const uint8_t CLASS_LENGTH = static_cast<uint8_t>( CLASS.size() );
	static const float HEADING( 213.44f );

	// Create source buffer.
	ServerProtocol::Buffer source;
	source.insert( source.end(), reinterpret_cast<const char*>( &ID ), reinterpret_cast<const char*>( &ID ) + sizeof( ID ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
	source.insert( source.end(), reinterpret_cast<const char*>( &CLASS_LENGTH ), reinterpret_cast<const char*>( &CLASS_LENGTH ) + sizeof( CLASS_LENGTH ) );
	source.insert( source.end(), reinterpret_cast<const char*>( CLASS.c_str() ), reinterpret_cast<const char*>( CLASS.c_str() ) + CLASS.size() );

	// Initial state.
	{
		msg::CreateEntity msg;

		BOOST_CHECK( msg.get_id() == 0 );
		BOOST_CHECK( msg.get_position() == Planet::Coordinate( 0, 0, 0 ) );
		BOOST_CHECK( msg.get_heading() == 0 );
		BOOST_CHECK( msg.get_class() == "" );
	}

	// Basic properties.
	{
		msg::CreateEntity msg;
		msg.set_id( ID );
		msg.set_position( POSITION );
		msg.set_heading( HEADING );
		msg.set_class( CLASS );

		BOOST_CHECK( msg.get_id() == ID );
		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_heading() == HEADING );
		BOOST_CHECK( msg.get_class() == CLASS );
	}

	// Serialize.
	{
		msg::CreateEntity msg;
		msg.set_id( ID );
		msg.set_position( POSITION );
		msg.set_heading( HEADING );
		msg.set_class( CLASS );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Serialize with invalid class ID.
	{
		msg::CreateEntity msg;
		msg.set_id( ID );
		msg.set_position( POSITION );
		msg.set_heading( HEADING );
		msg.set_class( "" );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::CreateEntity::InvalidDataException, ExceptionChecker<msg::CreateEntity::InvalidDataException>( "Invalid class name." ) );

		msg.set_class( std::string( 256, 'x' ) );
		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::CreateEntity::InvalidDataException, ExceptionChecker<msg::CreateEntity::InvalidDataException>( "Invalid class name." ) );
	}

	// Deserialize.
	{
		msg::CreateEntity msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );

		BOOST_CHECK( eaten == source.size() );
		BOOST_CHECK( msg.get_id() == ID );
		BOOST_CHECK( msg.get_position() == POSITION );
		BOOST_CHECK( msg.get_heading() == HEADING );
		BOOST_CHECK( msg.get_class() == CLASS );
	}

	// Deserialize with bogus class length.
	{
		uint8_t zero = 0;

		ServerProtocol::Buffer invalid_source;
		invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &ID ), reinterpret_cast<const char*>( &ID ) + sizeof( ID ) );
		invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &POSITION ), reinterpret_cast<const char*>( &POSITION ) + sizeof( POSITION ) );
		invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( &HEADING ), reinterpret_cast<const char*>( &HEADING ) + sizeof( HEADING ) );
		invalid_source.push_back( zero );
		invalid_source.insert( invalid_source.end(), reinterpret_cast<const char*>( CLASS.c_str() ), reinterpret_cast<const char*>( CLASS.c_str() ) + CLASS.size() );

		msg::CreateEntity msg;
		std::size_t eaten = 0;

		BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source.front(), invalid_source.size() ), msg::Beam::BogusDataException, ExceptionChecker<msg::Beam::BogusDataException>( "Invalid class length." ) );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with too less data.
	{
		msg::CreateEntity msg;

		for( std::size_t amount = 0; amount < source.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source[0], amount ) == 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( TestChatMessage ) {
	using namespace flex;

	static const sf::String MESSAGE = L"Hello, this is the operator!";
	static const uint16_t MESSAGE_SIZE = static_cast<uint8_t>( MESSAGE.getSize() );
	static const std::string SENDER = "Tank";
	static const uint8_t SENDER_SIZE = static_cast<uint8_t>( SENDER.size() );
	static const std::string TARGET = "Status";
	static const uint8_t TARGET_SIZE = static_cast<uint8_t>( TARGET.size() );

	// Create source buffer.
	ServerProtocol::Buffer source;
	source.insert( source.end(), reinterpret_cast<const char*>( &MESSAGE_SIZE ), reinterpret_cast<const char*>( &MESSAGE_SIZE ) + sizeof( MESSAGE_SIZE ) );
	source.insert( source.end(), reinterpret_cast<const char*>( MESSAGE.getData() ), reinterpret_cast<const char*>( MESSAGE.getData() ) + (MESSAGE.getSize() * sizeof( sf::Uint32 )) );
	source.insert( source.end(), reinterpret_cast<const char*>( &SENDER_SIZE ), reinterpret_cast<const char*>( &SENDER_SIZE ) + sizeof( SENDER_SIZE ) );
	source.insert( source.end(), reinterpret_cast<const char*>( SENDER.c_str() ), reinterpret_cast<const char*>( SENDER.c_str() ) + SENDER.size() );
	source.insert( source.end(), reinterpret_cast<const char*>( &TARGET_SIZE ), reinterpret_cast<const char*>( &TARGET_SIZE ) + sizeof( TARGET_SIZE ) );
	source.insert( source.end(), reinterpret_cast<const char*>( TARGET.c_str() ), reinterpret_cast<const char*>( TARGET.c_str() ) + TARGET.size() );

	// Initial state.
	{
		msg::Chat msg;

		BOOST_CHECK( msg.get_message() == "" );
		BOOST_CHECK( msg.get_sender() == "" );
		BOOST_CHECK( msg.get_target() == "" );
	}

	// Basic properties.
	{
		msg::Chat msg;
		msg.set_message( MESSAGE );
		msg.set_sender( SENDER );
		msg.set_target( TARGET );

		BOOST_CHECK( msg.get_message() == MESSAGE );
		BOOST_CHECK( msg.get_sender() == SENDER );
		BOOST_CHECK( msg.get_target() == TARGET );
	}

	// Serialize.
	{
		msg::Chat msg;
		msg.set_message( MESSAGE );
		msg.set_sender( SENDER );
		msg.set_target( TARGET );

		ServerProtocol::Buffer buffer;
		BOOST_CHECK_NO_THROW( msg.serialize( buffer ) );

		BOOST_CHECK( buffer == source );
	}

	// Serialize with invalid message.
	{
		msg::Chat msg;
		msg.set_sender( SENDER );
		msg.set_target( TARGET );

		ServerProtocol::Buffer buffer;

		// Empty message.
		msg.set_message( "" );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chat::InvalidDataException, ExceptionChecker<msg::Chat::InvalidDataException>( "Invalid message." ) );

		// Too long message.
		msg.set_message( std::string( 0x1ffff, 'x' ) );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chat::InvalidDataException, ExceptionChecker<msg::Chat::InvalidDataException>( "Invalid message." ) );
	}

	// Serialize with invalid sender.
	{
		msg::Chat msg;
		msg.set_message( MESSAGE );
		msg.set_target( TARGET );

		ServerProtocol::Buffer buffer;

		// Empty sender.
		msg.set_sender( "" );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chat::InvalidDataException, ExceptionChecker<msg::Chat::InvalidDataException>( "Invalid sender." ) );

		// Too long sender.
		msg.set_sender( std::string( 0x1ff, 'x' ) );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chat::InvalidDataException, ExceptionChecker<msg::Chat::InvalidDataException>( "Invalid sender." ) );
	}

	// Serialize with invalid target.
	{
		msg::Chat msg;
		msg.set_message( MESSAGE );
		msg.set_sender( SENDER );

		ServerProtocol::Buffer buffer;

		// Empty target.
		msg.set_target( "" );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chat::InvalidDataException, ExceptionChecker<msg::Chat::InvalidDataException>( "Invalid target." ) );

		// Too long target.
		msg.set_target( std::string( 0x1ff, 'x' ) );

		BOOST_CHECK_EXCEPTION( msg.serialize( buffer ), msg::Chat::InvalidDataException, ExceptionChecker<msg::Chat::InvalidDataException>( "Invalid target." ) );
	}

	// Deserialize.
	{
		msg::Chat msg;

		std::size_t eaten = 0;
		BOOST_CHECK_NO_THROW( eaten = msg.deserialize( &source[0], source.size() ) );

		BOOST_CHECK( eaten == source.size() );
		BOOST_CHECK( msg.get_message() == MESSAGE );
		BOOST_CHECK( msg.get_sender() == SENDER );
		BOOST_CHECK( msg.get_target() == TARGET );
	}

	// Deserialize with bogus message.
	{
		msg::Chat msg;

		ServerProtocol::Buffer invalid_source = source;

		// 0 length.
		invalid_source[0] = 0x00;
		invalid_source[1] = 0x00;

		std::size_t eaten = 0;
		BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source[0], invalid_source.size() ), msg::Chat::BogusDataException, ExceptionChecker<msg::Chat::BogusDataException>( "Invalid message length." ) );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with bogus sender.
	{
		msg::Chat msg;

		ServerProtocol::Buffer invalid_source = source;

		// 0 length.
		invalid_source[sizeof( uint16_t ) + (MESSAGE.getSize() * sizeof( sf::Uint32 ))] = 0x00;

		std::size_t eaten = 0;
		BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source[0], invalid_source.size() ), msg::Chat::BogusDataException, ExceptionChecker<msg::Chat::BogusDataException>( "Invalid sender length." ) );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with bogus target.
	{
		msg::Chat msg;

		ServerProtocol::Buffer invalid_source = source;

		// 0 length.
		invalid_source[sizeof( uint16_t ) + (MESSAGE.getSize() * sizeof( sf::Uint32 )) + sizeof( uint8_t ) + SENDER.size()] = 0x00;

		std::size_t eaten = 0;
		BOOST_CHECK_EXCEPTION( eaten = msg.deserialize( &invalid_source[0], invalid_source.size() ), msg::Chat::BogusDataException, ExceptionChecker<msg::Chat::BogusDataException>( "Invalid target length." ) );
		BOOST_CHECK( eaten == 0 );
	}

	// Deserialize with too less data.
	{
		msg::Chat msg;

		for( std::size_t amount = 0; amount < source.size(); ++amount ) {
			BOOST_CHECK( msg.deserialize( &source[0], amount ) == 0 );
		}
	}
}
