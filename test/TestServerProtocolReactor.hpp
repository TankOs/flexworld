#pragma once

#include <FlexWorld/ServerProtocol.hpp>

class TestServerProtocolReactor : public flex::ServerProtocol {
	public:
		TestServerProtocolReactor() :
			flex::ServerProtocol(),
			m_sender( nullptr )
		{
		}

		void handle_login_message( flex::Socket& sender, const std::string& username, const std::string& password ) {
			m_sender = &sender;
			m_login_username = username;
			m_login_password = password;
		}

		flex::Socket* m_sender;
		std::string m_login_username;
		std::string m_login_password;
};
