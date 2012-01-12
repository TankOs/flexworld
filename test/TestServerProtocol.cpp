#include <FlexWorld/ServerProtocol.hpp>
#include <FlexWorld/Messages/Login.hpp>

#include <boost/test/unit_test.hpp>

/*BOOST_AUTO_TEST_CASE( TestServerProtocol ) {
	using namespace flex;

	ServerProtocol protocol;

	// Login message.
	{
		Protocol::Buffer source;
		const std::string username( "Tank" );
		const std::string password( "h4x0r" );
		
		source.push_back( ServerProtocol::LOGIN ); // Opcode.
		source.push_back( static_cast<char>( username.size() ) );
		source.insert( source.end(), username.begin(), username.end() );
		source.push_back( static_cast<char>( password.size() ) );
		source.insert( source.end(), password.begin(), password.end() );

		// Serialize.
		{
			Protocol::Buffer buffer;
			msg:::Login

			BOOST_REQUIRE( protocol.serialize( source )
		}
	}
}*/
