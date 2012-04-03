#include "ConnectState.hpp"
#include "MenuState.hpp"
#include "PlayState.hpp"
#include "Shared.hpp"

#include <iostream>

ConnectState::ConnectState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target ),
	m_canceled( false ),
	m_go_on( false )
{
}

void ConnectState::init() {
	// GUI.
	// Widgets.
	m_info_window = sfg::Window::Create( sfg::Window::BACKGROUND );
	m_info_window->SetTitle( L"Connecting..." );

	m_info_label = sfg::Label::Create( L"" );

	// Layout.
	sfg::Box::Ptr vbox( sfg::Box::Create( sfg::Box::VERTICAL, 5.0f ) );
	vbox->Pack( m_info_label, true );

	m_info_window->Add( vbox );

	// Init.
	m_info_window->SetRequisition( sf::Vector2f( 400.f, 200.f ) );
	m_info_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ) / 2.f - m_info_window->GetAllocation().width / 2.f,
			static_cast<float>( get_render_target().getSize().y ) / 2.f - m_info_window->GetAllocation().height / 2.f
		)
	);

	m_desktop.Add( m_info_window );

	// Prepare backend.

	// Make sure IO service exists and is stopped.
	get_shared().io_service->stop();
	get_shared().io_service->reset();

	get_shared().client.reset( new flex::Client( *get_shared().io_service, *this ) );

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
		leave( new MenuState( get_render_target() ) );
	}
	else {
		m_desktop.HandleEvent( event );
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

	// Update GUI.
	if( m_next_info_text.size() ) {
		m_info_label->SetText( m_next_info_text );
		m_next_info_text.clear();
	}

	m_desktop.Update( delta.asSeconds() );

	if( m_go_on ) {
		leave( new PlayState( get_render_target() ) );
	}
}

void ConnectState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.clear();

	// Render GUI.
	sfg::Renderer::Get().Display( window );

	window.display();
}

void ConnectState::handle_connect( flex::Client::ConnectionID ) {
	m_next_info_text = "Connection established.";
}

void ConnectState::handle_disconnect( flex::Client::ConnectionID ) {
	m_next_info_text = "Connection failed.";
	m_canceled = true;
}

void ConnectState::handle_message( const flex::msg::ServerInfo& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	if( msg.get_auth_mode() == flex::msg::ServerInfo::OPEN_AUTH ) {
		// Send username & password. TODO: Ask for password for remote connections.
		flex::msg::OpenLogin login_msg;
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

void ConnectState::handle_message( const flex::msg::LoginOK& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	m_next_info_text = "Login successful, get ready!";

	get_shared().entity_id = msg.get_entity_id();

	m_go_on = true;
}
