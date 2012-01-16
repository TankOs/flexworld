#include <FlexWorld/ServerProtocol.hpp>
#include <FlexWorld/MessageHandler.hpp>
#include <FlexWorld/Messages/Login.hpp>

#include <boost/test/unit_test.hpp>

class Handler : public flex::MessageHandler<flex::ServerMessageList, flex::ServerProtocol::ConnectionID> {
	public:
		Handler() :
			m_login_handled( false )
		{
		}

		void handle_message( const flex::msg::Login& login, std::size_t sender ) {
			BOOST_CHECK( sender == 1337 );
			BOOST_CHECK( login.get_username() == "Tank" );
			BOOST_CHECK( login.get_password() == "h4x0r" );
			m_login_handled = true;
		}

		bool m_login_handled;
};

BOOST_AUTO_TEST_CASE( TestServerProtocol ) {
	using namespace flex;

	ServerProtocol protocol;
	Handler handler;

	// Dispatch unknown message.
	{
		ServerProtocol::Buffer buffer;
		std::size_t eaten = 0;

		BOOST_CHECK_THROW( eaten = protocol.dispatch( 254, buffer, handler, 1337 ), ServerProtocol::UnknownMessageIDException );
		BOOST_CHECK( eaten == 0 );
	}

	// Dispatch login message.
	{
		ServerProtocol::Buffer buffer;
		const std::string username( "Tank" );
		const std::string password( "h4x0r" );
		
		buffer.push_back( static_cast<char>( username.size() ) );
		buffer.insert( buffer.end(), username.begin(), username.end() );
		buffer.push_back( static_cast<char>( password.size() ) );
		buffer.insert( buffer.end(), password.begin(), password.end() );

		std::size_t eaten = 0;

		BOOST_CHECK_NO_THROW( eaten = protocol.dispatch( tpl::IndexOf<flex::msg::Login, ServerMessageList>::RESULT, buffer, handler, 1337 ) );
		BOOST_CHECK( eaten == 11 );
		BOOST_CHECK( handler.m_login_handled == true );
	}
}
