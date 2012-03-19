#include "MenuState.hpp"
#include "ConnectState.hpp"
#include "Shared.hpp"

#include <FlexWorld/Config.hpp>
#include <FlexWorld/GameModeDriver.hpp>
#include <FlexWorld/PackageEnumerator.hpp>
#include <FlexWorld/ClassDriver.hpp>
#include <FlexWorld/Log.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <ctime>
#include <cstdlib>

static const bool SHOW_NOTICES = false;
static const float FADE_SPEED = 2000.0f;
static const float SLIDE_TARGET_X = 50.f;

MenuState::MenuState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target ),
	m_fade_main_menu_out( false ),
	m_background_varray( sf::Quads, 4 )
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
	m_insta_button->OnClick.Connect( &MenuState::on_insta_click, this );
	m_start_game_button->OnClick.Connect( &MenuState::on_start_game_click, this );
	options_button->OnClick.Connect( &MenuState::on_options_click, this );
	quit_button->OnClick.Connect( &MenuState::on_quit_click, this );

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

	// Init clouds.
	sf::Image image;
	image.loadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/cloud.png" ) );
	m_cloud_texture.loadFromImage( image );

	for( uint8_t cloud_index = 0; cloud_index < 25; ++cloud_index ) {
		sf::Vector2f cloud_position(
			static_cast<float>( std::rand() % get_render_target().getSize().x ),
			static_cast<float>( std::rand() % get_render_target().getSize().y )
		);

		float scale( static_cast<float>( std::rand() % (100 - 9) + 10 ) / 100.f );

		sf::Sprite cloud_sprite( m_cloud_texture );
		cloud_sprite.setOrigin( static_cast<float>( m_cloud_texture.getWidth() / 2 ), static_cast<float>( m_cloud_texture.getHeight() / 2 ) );
		cloud_sprite.setPosition( cloud_position.x, cloud_position.y );
		cloud_sprite.setScale( scale, scale );
		cloud_sprite.setColor( sf::Color( 255, 255, 255, 160 ) );

		// Bigger clouds get added to the buffer behind smaller ones to prevent
		// being overdrawn by farer objects.
		std::size_t insert_idx = 0;

		for( insert_idx = 0; insert_idx < m_cloud_sprites.size(); ++insert_idx ) {
			if( cloud_sprite.getScale().x <= m_cloud_sprites[insert_idx].getScale().x ) {
				break;
			}
		}

		m_cloud_sprites.insert( m_cloud_sprites.begin() + insert_idx, cloud_sprite );
	}

	// Prepare background.
	float width = static_cast<float>( get_render_target().getSize().x );
	float height = static_cast<float>( get_render_target().getSize().y );
	sf::Uint8 alpha = 100;

	m_background_varray[0] = sf::Vertex( sf::Vector2f( 0, 0 ), sf::Color( 0x88, 0x88, 0x88, alpha ) );
	m_background_varray[1] = sf::Vertex( sf::Vector2f( 0, height ), sf::Color( 0xff, 0xff, 0xff, alpha ) );
	m_background_varray[2] = sf::Vertex( sf::Vector2f( width, height ), sf::Color( 0xff, 0xff, 0xff, alpha ) );
	m_background_varray[3] = sf::Vertex( sf::Vector2f( width, 0 ), sf::Color( 0x88, 0x88, 0x88, alpha ) );

	// Cleanup the backend.
	get_shared().account_manager.reset();
	get_shared().host.reset();
	get_shared().lock_facility.reset();
	get_shared().world.reset();

	get_render_target().resetGLStates();
}

void MenuState::cleanup() {
}

void MenuState::handle_event( const sf::Event& event ) {
	m_desktop.HandleEvent( event );

	// Check if options window ate the event.
	if( m_options_window && m_options_window->is_event_processed() ) {
		return;
	}

	if( event.type == sf::Event::Closed ) {
		leave();
	}

	if( (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) ) {
		if( m_fade_main_menu_out ) {
			m_fade_main_menu_out = false;
		}
		else {
			leave();
		}
	}
}

void MenuState::update( const sf::Time& delta ) {
	float seconds = delta.asSeconds();

	// Move clouds.
	for( std::size_t cloud_idx = 0; cloud_idx < m_cloud_sprites.size(); ++cloud_idx ) {
		sf::Sprite& cloud = m_cloud_sprites[cloud_idx];

		cloud.move( seconds * (20.f * cloud.getScale().x), 0.f /*seconds * -5.f*/ );

		if( cloud.getGlobalBounds().left >= static_cast<float>( get_render_target().getSize().x ) ) {
			cloud.move( -static_cast<float>( get_render_target().getSize().x ) - cloud.getGlobalBounds().width, 0.f );
		}

		if( cloud.getGlobalBounds().top + cloud.getGlobalBounds().height <= 0.f ) {
			cloud.move( 0.f, static_cast<float>( get_render_target().getSize().y ) + cloud.getGlobalBounds().height );
		}
	}

	// Update desktop.
	m_desktop.Update( seconds );

	// Slide windows.
	if( m_fade_main_menu_out ) {
		m_window->SetPosition(
			sf::Vector2f(
				std::max(
					-m_window->GetAllocation().width,
					m_window->GetAllocation().left - (seconds * FADE_SPEED)
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
							m_sliding_widget->GetAllocation().left - (FADE_SPEED * seconds)
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
					m_window->GetAllocation().left + (seconds * FADE_SPEED)
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
							m_sliding_widget->GetAllocation().left + (FADE_SPEED * seconds)
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

	window.draw( m_background_varray, sf::BlendMultiply );

	// Clouds.
	for( std::size_t cloud_idx = 0; cloud_idx < m_cloud_sprites.size(); ++cloud_idx ) {
		window.draw( m_cloud_sprites[cloud_idx], sf::BlendAlpha );
	}

	// Render GUI.
	sfg::Renderer::Get().Display( window );

	window.display();
}

void MenuState::on_options_click() {
	if( m_sliding_widget ) {
		return;
	}

	m_options_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ),
			m_window->GetAllocation().top
		)
	);

	m_options_window->refresh_user_settings( get_shared().user_settings );
	m_options_window->Show( true );

	m_fade_main_menu_out = true;
	m_sliding_widget = m_options_window;
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

	// Apply user settings.
	get_shared().user_settings = m_options_window->get_user_settings();
	get_shared().user_settings.save( UserSettings::get_profile_path() + "/settings.yml" );

	m_fade_main_menu_out = false;

	check_required_settings();

	// Apply vsync setting.
	get_render_target().setVerticalSyncEnabled( get_shared().user_settings.is_vsync_enabled() );

	// Apply resolution.
	if( reinit_window ) {
		get_render_target().create(
			get_shared().user_settings.get_video_mode(),
			"FlexWorld",
			get_shared().user_settings.is_fullscreen_enabled() ? sf::Style::Fullscreen : (sf::Style::Titlebar | sf::Style::Close)
		);

		// Restart menu state.
		leave( new MenuState( get_render_target() ) );
	}
}

void MenuState::on_options_reject() {
	m_fade_main_menu_out = false;
}

void MenuState::on_quit_click() {
	leave();
}

void MenuState::on_start_game_accept() {
	if( m_start_game_window->is_game_mode_selected() ) {
		// Create IO service.
		get_shared().io_service.reset( new boost::asio::io_service );

		// Prepare backend and session host.
		get_shared().account_manager.reset( new flex::AccountManager );
		get_shared().lock_facility.reset( new flex::LockFacility );
		get_shared().world.reset( new flex::World );

		get_shared().host.reset(
			new flex::SessionHost(
				*get_shared().io_service,
				*get_shared().lock_facility,
				*get_shared().account_manager,
				*get_shared().world
			)
		);

		get_shared().host->set_auth_mode( flex::SessionHost::OPEN_AUTH );

		// Load classes specified by selected game mode.
		const flex::GameMode& game_mode = m_start_game_window->get_selected_game_mode();
		bool load_ok = true;

		for( std::size_t package_idx = 0 ; package_idx < game_mode.get_num_packages(); ++package_idx ) {
			// Enumerate package.
			flex::PackageEnumerator enumerator;

			const flex::FlexID& package_id = game_mode.get_package( package_idx );
			std::string package_path = flex::ROOT_DATA_DIRECTORY + std::string( "packages/" ) + package_id.as_path();

			if( !enumerator.enumerate( package_path ) ) {
				flex::Log::Logger( flex::Log::WARNING ) << "Failed to load package from " << package_id.get() << "." << flex::Log::endl;
				continue;
			}

			// Load classes.
			flex::Log::Logger( flex::Log::DEBUG ) << "Loading " << enumerator.get_num_class_files() << " class(es) from " << package_id.get() << ":" << flex::Log::endl;

			for( std::size_t class_idx = 0; class_idx < enumerator.get_num_class_files(); ++class_idx ) {
				const std::string& filename = enumerator.get_class_file( class_idx );

				flex::Log::Logger( flex::Log::DEBUG ) << "-> " << filename << flex::Log::endl;

				try {
					flex::Class loaded_cls = flex::ClassDriver::load( filename );

					// Make sure same class wasn't added before.
					if( get_shared().world->find_class( loaded_cls.get_id() ) != nullptr ) {
						flex::Log::Logger( flex::Log::WARNING ) << "Duplicate class " << loaded_cls.get_id().get() << " loaded from package " << package_id.get() << ", skipping." << flex::Log::endl;
					}
					else {
						// Check that all referenced textures exist.
						for( std::size_t tex_idx = 0; tex_idx < loaded_cls.get_num_textures(); ++tex_idx ) {
							std::string file_path = flex::ROOT_DATA_DIRECTORY + std::string( "packages/" ) + loaded_cls.get_texture( tex_idx ).get_id().as_path();

							if( !boost::filesystem::exists( file_path ) ) {
								load_ok = false;

#if !defined( NDEBUG )
								std::cerr << "Texture not found: " << file_path << std::endl;
#endif
							}
						}

						// Check that model exists.
						{
							std::string model_path = flex::ROOT_DATA_DIRECTORY + std::string( "packages/" ) + loaded_cls.get_model().get_id().as_path();

							if( !boost::filesystem::exists( model_path ) ) {
								load_ok = false;

#if !defined( NDEBUG )
								std::cerr << "Model not found: " << model_path << std::endl;
#endif
							}
						}

						get_shared().world->add_class( loaded_cls );
						flex::Log::Logger( flex::Log::DEBUG ) << "--> " << loaded_cls.get_id().get() << flex::Log::endl;
					}
				}
				catch( const flex::ClassDriver::LoadException& /*e*/ ) {
					flex::Log::Logger( flex::Log::WARNING ) << "Failed to load class from " << filename << "." << flex::Log::endl;
					load_ok = false;
				}
			}
		}

		// If loading wasn't successful, cancel. TODO Show message.
		if( load_ok ) {
			// Head over to connect state.
			leave( new ConnectState( get_render_target() ) );
		}
		else {
			m_fade_main_menu_out = false;
		}
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

	m_start_game_button->Show( valid );
	m_join_game_button->Show( valid );
	m_settings_hint_label->Show( !valid );
}

void MenuState::on_insta_click() {
	on_start_game_accept();
}
