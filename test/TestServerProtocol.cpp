#include <FlexWorld/ServerProtocol.hpp>
#include <FlexWorld/MessageHandler.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>

#include <boost/test/unit_test.hpp>

class SPHandler : public fw::MessageHandler<fw::ServerMessageList, fw::ServerProtocol::ConnectionID> {
	public:
		using fw::MessageHandler<fw::ServerMessageList, fw::ServerProtocol::ConnectionID>::handle_message;

		SPHandler() :
			m_login_handled( false )
		{
		}

		void handle_message( const fw::msg::OpenLogin& login, fw::ServerProtocol::ConnectionID sender ) {
			BOOST_CHECK( sender == 9949 );
			BOOST_CHECK( login.get_username() == "Tank" );
			BOOST_CHECK( login.get_password() == "h4x0r" );
			BOOST_CHECK( login.get_server_password() == "me0w" );
			m_login_handled = true;
		}

		bool m_login_handled;
};

BOOST_AUTO_TEST_CASE( TestServerProtocol ) {
	using namespace fw;

	ServerProtocol protocol;
	SPHandler handler;

	// Incomplete message.
	{
		ServerProtocol::Buffer buffer( 1, 244 ); // Message ID only.
		BOOST_CHECK_THROW( protocol.dispatch( buffer, handler, 9949 ), ServerProtocol::BogusMessageDataException );
	}

	// Dispatch unknown message.
	{
		ServerProtocol::Buffer buffer( 2, 244 ); // 244 = unknown message ID.
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = protocol.dispatch( buffer, handler, 9949 ), ServerProtocol::UnknownMessageIDException );
		BOOST_CHECK( eaten == 0 );
	}

	// Dispatch known message.
	{
		ServerProtocol::Buffer buffer;
		const std::string username( "Tank" );
		const std::string password( "h4x0r" );
		const std::string server_password( "me0w" );
		
		buffer.push_back( static_cast<char>( tpl::IndexOf<fw::msg::OpenLogin, ServerMessageList>::RESULT ) );
		buffer.push_back( static_cast<char>( username.size() ) );
		buffer.insert( buffer.end(), username.begin(), username.end() );
		buffer.push_back( static_cast<char>( password.size() ) );
		buffer.insert( buffer.end(), password.begin(), password.end() );
		buffer.push_back( static_cast<char>( server_password.size() ) );
		buffer.insert( buffer.end(), server_password.begin(), server_password.end() );

		std::size_t eaten = 0;

		BOOST_CHECK_NO_THROW( eaten = protocol.dispatch( buffer, handler, 9949 ) );
		BOOST_CHECK( eaten == 17 );
		BOOST_CHECK( handler.m_login_handled == true );
	}
}
