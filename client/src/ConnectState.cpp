#include "ConnectState.hpp"
#include "StateFactory.hpp"
#include "Shared.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

ConnectState::ConnectState( sf::RenderWindow& target ) :
	State( target ),
	m_canceled( false ),
	m_go_on( false )
{
}

void ConnectState::init() {
	// Make sure IO service exists and is stopped.
	get_shared().io_service->stop();
	get_shared().io_service->reset();

	get_shared().client.reset( new fw::Client( *get_shared().io_service, *this ) );

	// If a session host exists, we want to launch a local server. If not, just
	// connect.
	if( get_shared().host ) {
		get_shared().host->start();
	}
	else {
		assert( 0 && "IMPLEMENT REMOTE CONNECTIONS" );
	}
}

void ConnectState::cleanup() {
	if( m_canceled ) {
		// Free all backend stuff.
		get_shared().client.reset();
		get_shared().lock_facility.reset();

		// Stop IO service.
		get_shared().io_service->stop();
		get_shared().io_service->reset();
	}
}

void ConnectState::handle_event( const sf::Event& event ) {
	if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape ) {
		m_canceled = true;
		leave( StateFactory::create_menu_state( get_render_target() ) );
	}
}

void ConnectState::update( const sf::Time& delta ) {
	// Poll the IO service.
	get_shared().io_service->poll();

	// Make connection when session host is running.
	if(
		get_shared().host != nullptr &&
		get_shared().client->is_started() == false
	) {
		m_next_info_text = sf::String( L"Making connection..." );

		get_shared().client->start( get_shared().host->get_ip(), get_shared().host->get_port() );
	}

	if( m_go_on ) {
		leave( StateFactory::create_play_state( get_render_target() ) );
	}
}

void ConnectState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.clear();

	window.display();
}

void ConnectState::handle_connect( fw::Client::ConnectionID ) {
	m_next_info_text = "Connection established.";
}

void ConnectState::handle_disconnect( fw::Client::ConnectionID ) {
	m_next_info_text = "Connection failed.";
	m_canceled = true;
}

void ConnectState::handle_message( const fw::msg::ServerInfo& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	if( msg.get_auth_mode() == fw::msg::ServerInfo::OPEN_AUTH ) {
		// Send username & password. TODO: Ask for password for remote connections.
		fw::msg::OpenLogin login_msg;
		login_msg.set_username( get_shared().user_settings.get_username() );
		login_msg.set_password( "LOCAL" );

		get_shared().client->send_message( login_msg );

		m_next_info_text = "Logging in...";
	}
	else {
		m_next_info_text = "Unknown auth type sent by server.";
		m_canceled = true;
	}
}

void ConnectState::handle_message( const fw::msg::LoginOK& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	m_next_info_text = "Login successful, get ready!";

	get_shared().entity_id = msg.get_entity_id();

	m_go_on = true;
}
