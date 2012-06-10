#include "MenuState.hpp"
#include "ConnectState.hpp"
#include "Shared.hpp"
#include "OptionsDocumentController.hpp"
#include "RocketRenderInterface.hpp"
#include "RocketSystemInterface.hpp"
#include "RocketEventDispatcher.hpp"
#include "RocketUtils.hpp"

#include <FlexWorld/Config.hpp>
#include <FlexWorld/GameModeDriver.hpp>
#include <FlexWorld/PackageEnumerator.hpp>
#include <FlexWorld/ClassDriver.hpp>
#include <FlexWorld/Log.hpp>

#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <ctime>
#include <cstdlib>

static const bool SHOW_NOTICES = false;
static const float FADE_SPEED = 3000.0f;
static const float SLIDE_TARGET_X = 50.f;

MenuState::MenuState( sf::RenderWindow& target ) :
	State( target ),
	m_fade_main_menu_out( false ),
	m_background_varray( sf::Quads, 4 ),
	m_rocket_context( nullptr ),
	m_options_document( nullptr )
{
}

void MenuState::init() {
	srand( static_cast<unsigned int>( time( nullptr ) ) );

	set_render_fps( 60 );

	// Widgets.
	sfg::Label::Ptr title_label( sfg::Label::Create( L"FlexWorld" ) );
	title_label->SetId( "title" );

	sfg::Label::Ptr version_label( sfg::Label::Create( L"FlexWorld" ) );
	{
		std::stringstream sstr;
		sstr
			<< "Version "
			<< static_cast<int>( flex::VERSION.get_major() ) << "."
			<< static_cast<int>( flex::VERSION.get_minor() ) << "."
			<< static_cast<int>( flex::VERSION.get_revision() ) << " "
			<< flex::VERSION_SUFFIX
		;

		version_label->SetText( sstr.str() );
	}

	m_insta_button = sfg::Button::Create( L"INSTA!!!" );
	m_start_game_button = sfg::Button::Create( L"Start game" );
	m_join_game_button = sfg::Button::Create( L"Join game" );
	sfg::Button::Ptr options_button( sfg::Button::Create( L"Options" ) );
	sfg::Button::Ptr quit_button( sfg::Button::Create( L"Quit" ) );

	sfg::Label::Ptr notice0( sfg::Label::Create( L"This is an \"in-development version\". You're not allowed" ) );
	sfg::Label::Ptr notice1( sfg::Label::Create( L"to distribute it in any way, talk about it or show any related" ) );
	sfg::Label::Ptr notice2( sfg::Label::Create( L"material without explicit permission!!!" ) );

	m_settings_hint_label = sfg::Label::Create( L"Please set your username and serial in the options dialog." );
	m_settings_hint_label->Show( false );

	notice0->SetClass( "important" );
	notice1->SetClass( "important" );
	notice2->SetClass( "important" );

	// Layout.
	sfg::Box::Ptr vbox( sfg::Box::Create( sfg::Box::VERTICAL, 10.f ) );
	vbox->Pack( title_label, false );
	vbox->Pack( version_label, false );
	vbox->Pack( sfg::Label::Create( L"http://flexworld-game.com/" ), false );
	vbox->Pack( m_insta_button, false );
	vbox->Pack( m_start_game_button, false );
	//vbox->Pack( m_join_game_button, false );
	vbox->Pack( options_button, false );
	vbox->Pack( quit_button, false );
	vbox->Pack( m_settings_hint_label, false );

	if( SHOW_NOTICES ) {
		vbox->Pack( notice0, false );
		vbox->Pack( notice1, false );
		vbox->Pack( notice2, false );
	}

	m_window = sfg::Window::Create();
	m_window->SetId( "menu" );
	m_window->SetTitle( L"FlexWorld" );
	m_window->SetStyle( sfg::Window::BACKGROUND );
	m_window->Add( vbox );

	m_desktop.Add( m_window );

	// Signals.
	m_insta_button->GetSignal( sfg::Button::OnLeftClick ).Connect( &MenuState::on_insta_click, this );
	m_start_game_button->GetSignal( sfg::Button::OnLeftClick ).Connect( &MenuState::on_start_game_click, this );
	options_button->GetSignal( sfg::Button::OnLeftClick ).Connect( &MenuState::on_options_click, this );
	quit_button->GetSignal( sfg::Button::OnLeftClick ).Connect( &MenuState::on_quit_click, this );

	// Init.
	m_desktop.LoadThemeFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/menu.theme" ) );
	check_required_settings();

	m_window->SetPosition(
		sf::Vector2f(
			SLIDE_TARGET_X,
			SLIDE_TARGET_X
		)
	);

	// Create options and start game windows.
	m_options_window = OptionsWindow::Create( get_shared().user_settings );
	sfg::DynamicPointerCast<OptionsWindow>( m_options_window )->OnAccept.Connect( &MenuState::on_options_accept, this );
	sfg::DynamicPointerCast<OptionsWindow>( m_options_window )->OnReject.Connect( &MenuState::on_options_reject, this );

	m_start_game_window = StartGameWindow::Create();
	sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->OnAccept.Connect( &MenuState::on_start_game_accept, this );
	sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->OnReject.Connect( &MenuState::on_start_game_reject, this );

	m_desktop.Add( m_options_window );
	m_desktop.Add( m_start_game_window );

	m_options_window->Show( false );
	m_start_game_window->Show( false );

	// Load and add game modes to dialog.
	using namespace boost::filesystem;

	directory_iterator dir_iter_end;
	directory_iterator dir_iter( flex::ROOT_DATA_DIRECTORY + std::string( "/modes/" ) );

	for( ; dir_iter != dir_iter_end; ++dir_iter ) {
		path bpath( *dir_iter );

		// Skip directories and files not ending with .yml.
		if( !is_regular( bpath ) || bpath.extension() != ".yml" ) {
			continue;
		}

		// Open file and read everything.
		std::ifstream in( bpath.c_str() );
		if( !in.is_open() ) {
			// Failed to open, skip.
			continue;
		}

		std::stringstream sstr;
		sstr << in.rdbuf();

		// Try to deserialize game mode.
		try {
			flex::GameMode mode = flex::GameModeDriver::deserialize( sstr.str() );

			// Worked, add to dialog.
			sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->add_game_mode( mode );
		}
		catch( const flex::GameModeDriver::DeserializeException& /*e*/ ) {
			// Skip.
			continue;
		}
	}

	// Prepare background.
	float width = static_cast<float>( get_render_target().getSize().x );
	float height = static_cast<float>( get_render_target().getSize().y );

	m_background_texture.loadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/bg.png" ) );

	sf::Vector2f texture_size(
		static_cast<float>( m_background_texture.getSize().x ),
		static_cast<float>( m_background_texture.getSize().y )
	);

	m_background_varray.resize(
		static_cast<unsigned int>( std::ceil( width / texture_size.x ) ) *
		static_cast<unsigned int>( std::ceil( height / texture_size.y ) ) *
		4
	);

	unsigned int varray_idx = 0;

	for( float y = 0.0f; y < height; y += texture_size.y ) {
		for( float x = 0.0f; x < width; x += texture_size.x ) {
			m_background_varray[varray_idx++] = sf::Vertex( sf::Vector2f( x, y ), sf::Vector2f( 0.0f, 0.0f ) );
			m_background_varray[varray_idx++] = sf::Vertex( sf::Vector2f( x, y + texture_size.y ), sf::Vector2f( 0.0f, texture_size.y ) );
			m_background_varray[varray_idx++] = sf::Vertex( sf::Vector2f( x + texture_size.x, y + texture_size.y ), sf::Vector2f( texture_size.x, texture_size.y ) );
			m_background_varray[varray_idx++] = sf::Vertex( sf::Vector2f( x + texture_size.x, y ), sf::Vector2f( texture_size.x, 0.0f ) );
		}
	}


	// Setup Rocket.
	RocketRenderInterface* render_interface = new RocketRenderInterface( get_render_target() );
	RocketSystemInterface* system_interface = new RocketSystemInterface;

	Rocket::Core::SetRenderInterface( render_interface );
	Rocket::Core::SetSystemInterface( system_interface );

	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	render_interface->RemoveReference();
	system_interface->RemoveReference();

	Rocket::Core::FontDatabase::LoadFontFace(
		(flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/Economica-Bold.ttf" )).c_str(),
		"MenuFont",
		Rocket::Core::Font::STYLE_NORMAL,
		Rocket::Core::Font::WEIGHT_NORMAL
	);

	m_rocket_context = Rocket::Core::CreateContext(
		"default",
		Rocket::Core::Vector2i(
			get_render_target().getSize().x,
			get_render_target().getSize().y
		)
	);

	Rocket::Debugger::Initialise( m_rocket_context );
	//Rocket::Debugger::SetVisible( true );

	const std::string filename = flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/main_menu.rml" );
	Rocket::Core::ElementDocument* document = m_rocket_context->LoadDocument( filename.c_str() );

	document->AddEventListener( "click", this );
	document->Show();

	center_element(
		*document->GetElementById( "main_menu" ),
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ) / 2.0f,
			static_cast<float>( get_render_target().getSize().y ) / 2.0f
		)
	);

	document->RemoveReference();

	// Setup options document + controller.
	m_options_document = m_rocket_context->LoadDocument( (flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/options.rml" )).c_str() );
	m_options_controller.reset( new OptionsDocumentController( *m_options_document ) );

	m_options_controller->on_close = std::bind( &MenuState::on_options_reject, this );

	// Music. XXX
	std::string music_path = flex::ROOT_DATA_DIRECTORY + std::string( "/local/music/kevin_macleod_-_cambodean_odessy.ogg" );

	if( boost::filesystem::exists( music_path ) ) {
		m_music.openFromFile( music_path );
		m_music.setLoop( true );
		//m_music.play();
	}

	// Cleanup the backend.
	get_shared().account_manager.reset();
	get_shared().host.reset();
	get_shared().lock_facility.reset();
	get_shared().world.reset();

	get_render_target().resetGLStates();
}

void MenuState::cleanup() {
	// Save window position.
	get_shared().user_settings.set_window_position( get_render_target().getPosition() );
	get_shared().user_settings.save( UserSettings::get_profile_path() + "/settings.yml" );

	// Cleanup Rocket.
	m_options_document->RemoveReference();
	m_rocket_context->RemoveReference();

	//Rocket::Core::Shutdown();
}

void MenuState::handle_event( const sf::Event& event ) {
	RocketEventDispatcher::dispatch_event( event, *m_rocket_context );
	//m_desktop.HandleEvent( event );

	// Check if options window ate the event.
	if( m_options_window && m_options_window->is_event_processed() ) {
		return;
	}

	if( event.type == sf::Event::Closed ) {
		leave();
	}

	if( event.type == sf::Event::KeyPressed ) {
		if( event.key.code == sf::Keyboard::Escape ) {
			if( m_fade_main_menu_out ) {
				m_fade_main_menu_out = false;
			}
			else {
				leave();
			}
		}
		else if( event.key.code == sf::Keyboard::Return ) { // XXX
			on_insta_click();
		}
	}

}

void MenuState::update( const sf::Time& delta ) {
	float seconds = delta.asSeconds();

	// Update desktop.
	m_desktop.Update( seconds );

	// Update rocket.
	m_rocket_context->Update();

	// Slide windows.
	float abs_fade_speed = FADE_SPEED + static_cast<float>( get_render_target().getSize().x ) / 2.0f;

	if( m_fade_main_menu_out ) {
		m_window->SetPosition(
			sf::Vector2f(
				std::max(
					-m_window->GetAllocation().width,
					m_window->GetAllocation().left - (seconds * abs_fade_speed)
				),
				m_window->GetAllocation().top
			)
		);

		if( m_sliding_widget ) {
			if( m_sliding_widget->GetAllocation().left > SLIDE_TARGET_X ) {
				m_sliding_widget->SetPosition(
					sf::Vector2f(
						std::max(
							SLIDE_TARGET_X,
							m_sliding_widget->GetAllocation().left - (abs_fade_speed * seconds)
						),
						m_sliding_widget->GetAllocation().top
					)
				);
			}
		}
	}
	else {
		m_window->SetPosition(
			sf::Vector2f(
				std::min(
					50.f,
					m_window->GetAllocation().left + (seconds * abs_fade_speed)
				),
				m_window->GetAllocation().top
			)
		);

		if( m_sliding_widget ) {
			if( m_sliding_widget->GetAllocation().left < static_cast<float>( get_render_target().getSize().x ) ) {
				m_sliding_widget->SetPosition(
					sf::Vector2f(
						std::min(
							static_cast<float>( get_render_target().getSize().x ),
							m_sliding_widget->GetAllocation().left + (abs_fade_speed * seconds)
						),
						m_sliding_widget->GetAllocation().top
					)
				);
			}

			if( m_sliding_widget->GetAllocation().left >= static_cast<float>( get_render_target().getSize().x ) ) {
				m_sliding_widget->Show( false );
				m_sliding_widget.reset();
			}
		}
	}
}

void MenuState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.clear( sf::Color( 0x66, 0x9c, 0xff ) );

	{
		sf::RenderStates states;
		states.texture = &m_background_texture;

		window.draw( m_background_varray, states );
	}

	// Render GUI.
	//sfg::Renderer::Get().Display( window );

	window.pushGLStates();
	m_rocket_context->Render();
	window.popGLStates();

	window.display();
}

void MenuState::on_options_click() {
	if( m_options_document->IsVisible() ) {
		return;
	}

	// Serialize current settings.
	m_options_controller->serialize( get_shared().user_settings );

	m_options_document->Show( Rocket::Core::ElementDocument::MODAL );

	// Get window and center it.
	Rocket::Core::Element* window_element = m_options_document->GetElementById( "window" );
	assert( window_element );

	center_element(
		*window_element,
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ) / 2.0f,
			static_cast<float>( get_render_target().getSize().y ) / 2.0f
		)
	);
}

void MenuState::on_start_game_click() {
	if( m_sliding_widget ) {
		return;
	}

	m_start_game_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ),
			m_window->GetAllocation().top
		)
	);

	m_fade_main_menu_out = true;
	m_sliding_widget = m_start_game_window;
	m_start_game_window->Show( true );
}

void MenuState::on_options_accept() {
	// Do we need to reinitialize the window?
	bool reinit_window =
		get_shared().user_settings.get_video_mode().width != m_options_window->get_user_settings().get_video_mode().width ||
		get_shared().user_settings.get_video_mode().height != m_options_window->get_user_settings().get_video_mode().height ||
		get_shared().user_settings.get_video_mode().bitsPerPixel != m_options_window->get_user_settings().get_video_mode().bitsPerPixel ||
		get_shared().user_settings.is_fullscreen_enabled() != m_options_window->get_user_settings().is_fullscreen_enabled()
	;

	// Remember old fullscreen state.
	bool was_fullscreen = get_shared().user_settings.is_fullscreen_enabled();

	// Apply user settings.
	get_shared().user_settings = m_options_window->get_user_settings();

	m_fade_main_menu_out = false;

	check_required_settings();

	// Apply vsync setting.
	get_render_target().setVerticalSyncEnabled( get_shared().user_settings.is_vsync_enabled() );

	// Apply resolution.
	if( reinit_window ) {
		// Remember position and fullscreen state.
		sf::Vector2i window_position = get_render_target().getPosition();

		get_render_target().create(
			get_shared().user_settings.get_video_mode(),
			"FlexWorld",
			get_shared().user_settings.is_fullscreen_enabled() ? sf::Style::Fullscreen : (sf::Style::Titlebar | sf::Style::Close)
		);

		// Restore old position when new video settings don't want fullscreen. If
		// fullscreen do nothing, the OS will handle it.
		if( get_shared().user_settings.is_fullscreen_enabled() == false ) {
			// If fullscreen was enabled before, move window to screen center.
			if( was_fullscreen == true ) {
				get_render_target().setPosition(
					sf::Vector2i(
						(sf::VideoMode::getDesktopMode().width - get_shared().user_settings.get_video_mode().width) / 2,
						(sf::VideoMode::getDesktopMode().height - get_shared().user_settings.get_video_mode().height) / 2
					)
				);
			}
			else {
				// If not fullscreen before, restore old position.
				get_render_target().setPosition( window_position );
			}

		}

		// Save position.
		get_shared().user_settings.set_window_position( get_render_target().getPosition() );

		// Restart menu state.
		leave( new MenuState( get_render_target() ) );
	}

	// Save settings.
	get_shared().user_settings.save( UserSettings::get_profile_path() + "/settings.yml" );
}

void MenuState::on_options_reject() {
	m_options_document->Show( Rocket::Core::ElementDocument::FOCUS );
	m_options_document->Hide();
}

void MenuState::on_quit_click() {
	leave();
}

void MenuState::on_start_game_accept() {
	if( m_start_game_window->is_game_mode_selected() ) {
		// Create IO service.
		get_shared().io_service.reset( new boost::asio::io_service );

		// Get selected game mode.
		const flex::GameMode& game_mode = m_start_game_window->get_selected_game_mode();

		// Prepare backend and session host.
		get_shared().account_manager.reset( new flex::AccountManager );
		get_shared().lock_facility.reset( new flex::LockFacility );
		get_shared().world.reset( new flex::World );

		get_shared().host.reset(
			new flex::SessionHost(
				*get_shared().io_service,
				*get_shared().lock_facility,
				*get_shared().account_manager,
				*get_shared().world,
				game_mode
			)
		);

		// Add search path.
		get_shared().host->add_search_path( flex::ROOT_DATA_DIRECTORY + std::string( "packages" ) );

		// Set auth mode.
		get_shared().host->set_auth_mode( flex::SessionHost::OPEN_AUTH );

		// Set endpoint.
		get_shared().host->set_ip( "127.0.0.1" );

		// Find free port.
		bool in_use = true;
		unsigned short port = 2593;
		boost::asio::io_service io_service;

		while( in_use ) {
			boost::asio::ip::tcp::socket prober( io_service );
			boost::asio::ip::tcp::endpoint endpoint(
				boost::asio::ip::address::from_string( get_shared().host->get_ip( ) ),
				port
			);

			try {
				prober.connect( endpoint );

				// Connection successful, try next port.
				++port;
			}
			catch( const boost::system::system_error& /*e*/ ) {
				// Connection failed, use port.
				in_use = false;
			}
		}

		get_shared().host->set_port( port );

		leave( new ConnectState( get_render_target() ) );
	}

}

void MenuState::on_start_game_reject() {
	m_fade_main_menu_out = false;
}

void MenuState::check_required_settings() {
	bool valid( true );

	if(
		get_shared().user_settings.get_username().empty() ||
		get_shared().user_settings.get_serial().empty()
	) {
		valid = false;
	}

	m_insta_button->Show( valid );
	m_start_game_button->Show( valid );
	m_join_game_button->Show( valid );
	m_settings_hint_label->Show( !valid );
}

void MenuState::on_insta_click() {
	on_start_game_accept();
}

void MenuState::ProcessEvent( Rocket::Core::Event& event ) {
	Rocket::Core::Element* element = event.GetTargetElement();

	if( element->GetId() == "quickstart" ) {
		on_insta_click();
	}
	else if( element->GetId() == "quit" ) {
		on_quit_click();
	}
	else if( element->GetId() == "options" ) {
		on_options_click();
	}

}
