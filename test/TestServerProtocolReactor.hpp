#if 0
#pragma once

#include <FlexWorld/ServerProtocol.hpp>

class TestServerProtocolReactor : public fw::ServerProtocol {
	public:
		TestServerProtocolReactor() :
			fw::ServerProtocol(),
			m_num_logins_received( 0 ),
			m_sender( 0 )
		{
		}

		void handle_login_message( ConnectionID sender, const std::string& username, const std::string& password ) {
			m_sender = sender;
			m_login_username = username;
			m_login_password = password;

			++m_num_logins_received;
		}

		std::string m_login_username;
		std::string m_login_password;
		std::size_t m_num_logins_received;
		ConnectionID m_sender;
};

#endif
