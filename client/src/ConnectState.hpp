#pragma once

#include "State.hpp"

#include <FlexWorld/Client.hpp>

/** Connect state.
 *
 * Connects to server, asks for username & password and synchronizes data
 * files.
 */
class ConnectState : public State, public flex::Client::Handler {
	public:
		/** Ctor.
		 * @param target Rendering target.
		 */
		ConnectState( sf::RenderWindow& target );

	private:
		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( const sf::Time& delta );
		void render() const;

		void handle_connect( flex::Client::ConnectionID );
		void handle_disconnect( flex::Client::ConnectionID );

		void handle_message( const flex::msg::ServerInfo& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::LoginOK& msg, flex::Client::ConnectionID conn_id );

		std::string m_next_info_text;

		bool m_canceled;
		bool m_go_on;
};
