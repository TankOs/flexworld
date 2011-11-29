#include "MenuState.hpp"
#include "StartGameWindow.hpp"

#include <FlexWorld/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <ctime>
#include <cstdlib>

MenuState::MenuState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target )
{
}

void MenuState::init() {
	srand( static_cast<unsigned int>( time( nullptr ) ) );

	// Widgets.
	sfg::Label::Ptr title_label( sfg::Label::Create( L"FlexWorld" ) );
	title_label->SetId( "title" );

	sfg::Label::Ptr version_label( sfg::Label::Create( L"FlexWorld" ) );
	{
		std::stringstream sstr;
		sstr
			<< "Version "
			<< static_cast<int>( flex::VERSION_MAJOR ) << "."
			<< static_cast<int>( flex::VERSION_MINOR ) << "."
			<< static_cast<int>( flex::VERSION_REVISION ) << " "
			<< flex::VERSION_SUFFIX
		;

		version_label->SetText( sstr.str() );
	}

	sfg::Button::Ptr start_game_button( sfg::Button::Create( L"Start game" ) );
	sfg::Button::Ptr join_game_button( sfg::Button::Create( L"Join game" ) );
	sfg::Button::Ptr options_button( sfg::Button::Create( L"Options" ) );
	sfg::Button::Ptr quit_button( sfg::Button::Create( L"Quit" ) );

	sfg::Label::Ptr notice0( sfg::Label::Create( L"This is an \"in-development version\". You're not allowed" ) );
	sfg::Label::Ptr notice1( sfg::Label::Create( L"to distribute it in any way, talk about it or show any related" ) );
	sfg::Label::Ptr notice2( sfg::Label::Create( L"material without explicit permission!!!" ) );

	notice0->SetClass( "important" );
	notice1->SetClass( "important" );
	notice2->SetClass( "important" );

	// Layout.
	sfg::Box::Ptr vbox( sfg::Box::Create( sfg::Box::VERTICAL, 10.f ) );
	vbox->Pack( title_label, false );
	vbox->Pack( version_label, false );
	vbox->Pack( sfg::Label::Create( L"http://flexworld-game.com/" ), false );
	vbox->Pack( start_game_button, false );
	vbox->Pack( join_game_button, false );
	vbox->Pack( options_button, false );
	vbox->Pack( quit_button, false );
	vbox->Pack( notice0, false );
	vbox->Pack( notice1, false );
	vbox->Pack( notice2, false );

	sfg::Window::Ptr window( sfg::Window::Create() );
	window->SetId( "menu" );
	window->SetTitle( L"FlexWorld" );
	window->SetBorderWidth( 15.f );
	window->SetStyle( sfg::Window::Background );
	window->Add( vbox );

	m_desktop.Add( window );

	// Signals.
	start_game_button->OnClick.Connect( &MenuState::on_start_game_click, this );
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

	// Music.
	m_music.OpenFromFile( "data/rebirth.ogg" );
	m_music.SetLoop( true );
	//m_music.Play();

	// Init clouds.
	sf::Image image;
	image.LoadFromFile( "data/gui/cloud.png" );
	m_cloud_texture.LoadFromImage( image );

	for( uint8_t cloud_index = 0; cloud_index < 25; ++cloud_index ) {
		uint8_t num_parts( static_cast<uint8_t>( std::rand() % (10 - 4) + 5 ) );
		sf::Vector2f cloud_position(
			static_cast<float>( std::rand() % get_render_target().GetWidth() ),
			static_cast<float>( std::rand() % get_render_target().GetHeight() )
		);

		for( uint8_t part_index = 0; part_index < num_parts; ++part_index ) {
			float angle( static_cast<float>( std::rand() % 3600) / 10.f );
			float scale( static_cast<float>( std::rand() % (100 - 49) + 50 ) / 100.f );
			uint8_t alpha( static_cast<uint8_t>( std::rand() % (255 - 149) + 150 ) );
			sf::Vector2f offset(
				static_cast<float>( std::rand() % 50 ),
				static_cast<float>( std::rand() % 20 )
			);

			sf::Sprite cloud_sprite( m_cloud_texture );
			cloud_sprite.SetOrigin( static_cast<float>( m_cloud_texture.GetWidth() / 2 ), static_cast<float>( m_cloud_texture.GetHeight() / 2 ) );
			cloud_sprite.SetPosition( cloud_position.x + offset.x, cloud_position.y + offset.y );
			cloud_sprite.SetScale( scale, scale );
			cloud_sprite.SetRotation( angle );
			cloud_sprite.SetBlendMode( sf::Blend::Add );
			cloud_sprite.SetColor( sf::Color( 255, 255, 255, alpha ) );

			m_cloud_sprites.push_back( cloud_sprite );
		}
	}

	// Setup sky.
	m_morphers[0].start( sf::Color( 0x15, 0x7f, 0xd0 ), sf::Color( 0x00, 0x00, 0x00 ), 60000 );
	m_morphers[1].start( sf::Color( 0x92, 0xc5, 0xec ), sf::Color( 0x15, 0x43, 0x66 ), 60000 );

	m_sky_shape.AddPoint( static_cast<float>( get_render_target().GetWidth() ), 0.f, m_morphers[0].get_current_color() );
	m_sky_shape.AddPoint( 0.f, 0.f, m_morphers[0].get_current_color() );
	m_sky_shape.AddPoint( 0.f, static_cast<float>( get_render_target().GetHeight() ), m_morphers[1].get_current_color() );
	m_sky_shape.AddPoint( static_cast<float>( get_render_target().GetWidth() ), static_cast<float>( get_render_target().GetHeight() ), m_morphers[1].get_current_color() );
}

void MenuState::cleanup() {
}

void MenuState::handle_event( const sf::Event& event ) {
	m_desktop.HandleEvent( event );

	// Check if options window ate the event.
	if( m_options_window && m_options_window->is_event_processed() ) {
		return;
	}

	if(
		event.Type == sf::Event::Closed ||
		(event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::Escape)
	) {
		leave();
		return;
	}

}

void MenuState::update( uint32_t delta ) {
	float f_time( static_cast<float>( delta ) / 1000.f );

	// Move clouds.
	SpriteList::iterator cloud_iter( m_cloud_sprites.begin() );
	SpriteList::iterator cloud_iter_end( m_cloud_sprites.end() );

	for( ; cloud_iter != cloud_iter_end; ++cloud_iter ) {
		cloud_iter->Move( f_time * 10.f, f_time * -5.f );

		if( cloud_iter->GetPosition().x - cloud_iter->GetSize().x / 2.f >= static_cast<float>( get_render_target().GetWidth() ) ) {
			cloud_iter->Move( -static_cast<float>( get_render_target().GetWidth() ) - cloud_iter->GetSize().x, 0.f );
		}

		if( cloud_iter->GetPosition().y + cloud_iter->GetSize().x / 2.f <= 0.f ) {
			cloud_iter->Move( 0.f, static_cast<float>( get_render_target().GetHeight() ) + cloud_iter->GetSize().x );
		}
	}

	// Morph and update sky colors.
	m_morphers[0].update( delta );
	m_morphers[1].update( delta );

	m_sky_shape.SetPointColor( 0, m_morphers[0].get_current_color() );
	m_sky_shape.SetPointColor( 1, m_morphers[0].get_current_color() );
	m_sky_shape.SetPointColor( 2, m_morphers[1].get_current_color() );
	m_sky_shape.SetPointColor( 3, m_morphers[1].get_current_color() );
}

void MenuState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.Clear( sf::Color( 0x12, 0x34, 0x56 ) );

	// Sky.
	window.Draw( m_sky_shape );

	// Clouds.
	SpriteList::const_iterator cloud_iter( m_cloud_sprites.begin() );
	SpriteList::const_iterator cloud_iter_end( m_cloud_sprites.end() );

	for( ; cloud_iter != cloud_iter_end; ++cloud_iter ) {
		window.Draw( *cloud_iter );
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
			static_cast<float>( get_render_target().GetHeight() ) / 2.f - m_options_window->GetAllocation().Height / 2.f
		)
	);

	sfg::DynamicPointerCast<OptionsWindow>( m_options_window )->OnAccept.Connect( &MenuState::on_options_accept, this );
	sfg::DynamicPointerCast<OptionsWindow>( m_options_window )->OnReject.Connect( &MenuState::on_options_reject, this );
}

void MenuState::on_start_game_click() {
	if( m_start_game_window ) {
		return;
	}

	m_start_game_window = StartGameWindow::Create();
	m_desktop.Add( m_start_game_window );

	m_start_game_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().GetWidth() ) / 2.f - m_start_game_window->GetAllocation().Width / 2.f,
			static_cast<float>( get_render_target().GetHeight() ) / 2.f - m_start_game_window->GetAllocation().Height / 2.f
		)
	);

	sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->OnAccept.Connect( &MenuState::on_start_game_accept, this );
	sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->OnReject.Connect( &MenuState::on_start_game_reject, this );
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

void MenuState::on_start_game_accept() {
	m_desktop.Remove( m_start_game_window );
	m_start_game_window.reset();
}

void MenuState::on_start_game_reject() {
	m_desktop.Remove( m_start_game_window );
	m_start_game_window.reset();
}
