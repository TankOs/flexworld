#include "ConnectState.hpp"
#include "MenuState.hpp"
#include "PlayState.hpp"
#include "Shared.hpp"

ConnectState::ConnectState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target ),
	m_canceled( false )
{
}

void ConnectState::init() {
	// GUI.
	// Widgets.
	m_info_window = sfg::Window::Create( sfg::Window::Background );
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
			static_cast<float>( get_render_target().GetWidth() ) / 2.f - m_info_window->GetAllocation().Width / 2.f,
			static_cast<float>( get_render_target().GetHeight() ) / 2.f - m_info_window->GetAllocation().Height / 2.f
		)
	);

	m_desktop.Add( m_info_window );

	// Prepare backend.
	assert( get_shared().host_thread == nullptr );
	assert( get_shared().client_thread == nullptr );
	assert( get_shared().client == nullptr );

	get_shared().client.reset( new flex::Client( *this ) );

	// If a session host exists, we want to launch a local server. If not, just
	// connect.
	if( get_shared().host ) {
		get_shared().host_thread.reset( new boost::thread( std::bind( &ConnectState::session_host_func, this ) ) );
		m_next_info_text = "Preparing game...";
	}
	else {
		assert( 0 && "IMPLEMENT REMOTE CONNECTIONS" );
	}
}

void ConnectState::cleanup() {
	if( m_canceled ) {
		// Close connections and wait for threads to complete.
		if( get_shared().client->is_connected() ) {
			get_shared().client->stop();
		}

		if( get_shared().client_thread ) {
			get_shared().client_thread->join();
		}

		if( get_shared().host ) {
			if( get_shared().host->is_running() ) {
				get_shared().host->stop();
			}

			if( get_shared().host_thread ) {
				get_shared().host_thread->join();
			}
		}

		// Free all backend stuff.
		get_shared().client.reset();
		get_shared().lock_facility.reset();
		get_shared().client_thread.reset();
		get_shared().host_thread.reset();
	}

}

void ConnectState::handle_event( const sf::Event& event ) {
	if( event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::Escape ) {
		m_canceled = true;
		leave( new MenuState( get_render_target() ) );
	}
	else {
		m_desktop.HandleEvent( event );
	}
}

void ConnectState::update( const sf::Time& delta ) {
	// Make connection when session host is running.
	if( get_shared().client_thread == nullptr && get_shared().host != nullptr && get_shared().host->is_running() ) {
		get_shared().client_thread.reset( new boost::thread( std::bind( &ConnectState::connect_func, this ) ) );
		m_next_info_text = "Game ready, connecting...";
	}

	// Update GUI.
	if( m_next_info_text.size() ) {
		m_info_label->SetText( m_next_info_text );
		m_next_info_text.clear();
	}

	m_desktop.Update( delta.AsSeconds() );
}

void ConnectState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.Clear();

	// Render GUI.
	sfg::Context::Get().GetRenderer().Display( window );

	window.Display();
}

void ConnectState::connect_func() {
	get_shared().client->run( "127.0.0.1", 2593 );
}

void ConnectState::handle_connect( flex::Client::ConnectionID ) {
	m_next_info_text = "Connection established.";
}

void ConnectState::handle_disconnect( flex::Client::ConnectionID ) {
	m_next_info_text = "Connection failed.";
	m_canceled = true;
}

void ConnectState::session_host_func() {
	// Run session host.
	get_shared().host->run();
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

void ConnectState::handle_message( const flex::msg::LoginOK& /*msg*/, flex::Client::ConnectionID /*conn_id*/ ) {
	m_next_info_text = "Login successful, get ready!";

	leave( new PlayState( get_render_target() ) );
}
