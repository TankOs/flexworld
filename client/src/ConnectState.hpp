#pragma once

#include "State.hpp"

#include <FlexWorld/Client.hpp>
#include <SFGUI/SFGUI.hpp>

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
		void update( uint32_t delta );
		void render() const;

		void session_host_func();

		void connect_func();
		void handle_connect( flex::Client::ConnectionID );
		void handle_disconnect( flex::Client::ConnectionID );
		void handle_message( const flex::msg::ServerInfo& msg, flex::Client::ConnectionID conn_id );

		sfg::Desktop m_desktop;
		std::string m_next_info_text;

		bool m_canceled;

		sfg::Window::Ptr m_info_window;
		sfg::Label::Ptr m_info_label;
};
