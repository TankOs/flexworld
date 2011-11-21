#include "MenuState.hpp"
#include "OptionsWindow.hpp"

#include <FlexWorld/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

MenuState::MenuState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target )
{
}

void MenuState::init() {
	// Widgets.
	sfg::Label::Ptr title_label( sfg::Label::Create( L"FlexWorld" ) );
	title_label->SetId( "title" );

	sfg::Label::Ptr version_label( sfg::Label::Create( L"FlexWorld" ) );
	{
		std::stringstream sstr;
		sstr
			<< static_cast<int>( flex::VERSION_MAJOR ) << "."
			<< static_cast<int>( flex::VERSION_MINOR ) << "."
			<< static_cast<int>( flex::VERSION_REVISION ) << " "
			<< flex::VERSION_SUFFIX
		;

		version_label->SetText( sstr.str() );
	}

	sfg::Button::Ptr play_button( sfg::Button::Create( L"Play" ) );
	sfg::Button::Ptr options_button( sfg::Button::Create( L"Options" ) );
	sfg::Button::Ptr quit_button( sfg::Button::Create( L"Quit" ) );

	// Layout.
	sfg::Box::Ptr vbox( sfg::Box::Create( sfg::Box::Vertical, 10.f ) );
	vbox->Pack( title_label, false );
	vbox->Pack( version_label, false );
	vbox->Pack( play_button, false );
	vbox->Pack( options_button, false );
	vbox->Pack( quit_button, false );

	sfg::Window::Ptr window( sfg::Window::Create() );
	window->SetTitle( L"FlexWorld" );
	window->SetBorderWidth( 15.f );
	window->SetStyle( sfg::Window::Background );
	window->Add( vbox );

	m_desktop.Add( window );

	// Signals.
	options_button->OnClick.Connect( &MenuState::on_options_click, this );
	quit_button->OnClick.Connect( &MenuState::on_quit_click, this );

	// Init.
	m_desktop.LoadThemeFromFile( "data/gui/menu.theme" );

	window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().GetWidth() ) / 2.f - window->GetAllocation().Width / 2.f,
			static_cast<float>( get_render_target().GetHeight() ) / 2.f - window->GetAllocation().Height / 2.f
		)
	);

	// Load background.
	sf::Image image;
	image.LoadFromFile( "data/gui/bg.png" );
	m_bg_texture.LoadFromImage( image );
}

void MenuState::cleanup() {
}

void MenuState::handle_event( const sf::Event& event ) {
	if(
		event.Type == sf::Event::Closed ||
		(event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::Escape)
	) {
		leave();
		return;
	}

	m_desktop.HandleEvent( event );
}

void MenuState::update( uint32_t /*delta*/ ) {
}

void MenuState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.Clear( sf::Color( 0x12, 0x34, 0x56 ) );

	// Background.
	sf::Sprite bg_tile( m_bg_texture );
	for( float y = 0.f; y < static_cast<float>( window.GetHeight() ); y += static_cast<float>( m_bg_texture.GetHeight() ) ) {
		for( float x = 0.f; x < static_cast<float>( window.GetWidth() ); x += static_cast<float>( m_bg_texture.GetWidth() ) ) {
			bg_tile.SetPosition( x, y );
			window.Draw( bg_tile );
		}
	}

	m_desktop.Expose( window );

	window.Display();
}

void MenuState::on_options_click() {
	if( m_options_window ) {
		return;
	}

	m_options_window = OptionsWindow::Create();
	m_desktop.Add( m_options_window );

	m_options_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().GetWidth() ) / 2.f - m_options_window->GetAllocation().Width / 2.f,
			static_cast<float>( get_render_target().GetHeight() ) / 2.f -m_options_window->GetAllocation().Height / 2.f
		)
	);

	std::dynamic_pointer_cast<OptionsWindow>( m_options_window )->OnAccept.Connect( &MenuState::on_options_accept, this );
	std::dynamic_pointer_cast<OptionsWindow>( m_options_window )->OnReject.Connect( &MenuState::on_options_reject, this );
}

void MenuState::on_options_accept() {
	m_desktop.Remove( m_options_window );
	m_options_window.reset();
}

void MenuState::on_options_reject() {
	m_desktop.Remove( m_options_window );
	m_options_window.reset();
}

void MenuState::on_quit_click() {
	leave();
}
