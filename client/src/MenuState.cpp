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

const bool SHOW_NOTICES = false;

MenuState::MenuState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target )
{
}

void MenuState::init() {
	srand( static_cast<unsigned int>( time( nullptr ) ) );

	set_render_fps( 60 );
	set_logic_fps( 60 );

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
	m_start_game_button->OnClick.Connect( &MenuState::on_start_game_click, this );
	options_button->OnClick.Connect( &MenuState::on_options_click, this );
	quit_button->OnClick.Connect( &MenuState::on_quit_click, this );

	// Init.
	m_desktop.LoadThemeFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/menu.theme" ) );
	check_required_settings();

	m_window->SetRequisition( sf::Vector2f( 400.f, 0.f ) );
	m_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ) / 2.f - m_window->GetAllocation().width / 2.f,
			static_cast<float>( get_render_target().getSize().y ) / 2.f - m_window->GetAllocation().height / 2.f
		)
	);

	// Init clouds.
	sf::Image image;
	image.loadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/cloud.png" ) );
	m_cloud_texture.loadFromImage( image );

	for( uint8_t cloud_index = 0; cloud_index < 25; ++cloud_index ) {
		uint8_t num_parts( static_cast<uint8_t>( std::rand() % (10 - 4) + 5 ) );
		sf::Vector2f cloud_position(
			static_cast<float>( std::rand() % get_render_target().getSize().x ),
			static_cast<float>( std::rand() % get_render_target().getSize().y )
		);

		for( uint8_t part_index = 0; part_index < num_parts; ++part_index ) {
			float angle( static_cast<float>( std::rand() % 3600) / 10.f );
			float scale( static_cast<float>( std::rand() % (100 - 49) + 50 ) / 100.f );
			sf::Vector2f offset(
				static_cast<float>( std::rand() % 50 ),
				static_cast<float>( std::rand() % 20 )
			);

			sf::Sprite cloud_sprite( m_cloud_texture );
			cloud_sprite.setOrigin( static_cast<float>( m_cloud_texture.getWidth() / 2 ), static_cast<float>( m_cloud_texture.getHeight() / 2 ) );
			cloud_sprite.setPosition( cloud_position.x + offset.x, cloud_position.y + offset.y );
			cloud_sprite.setScale( scale, scale );
			cloud_sprite.setRotation( angle );
			cloud_sprite.setColor( sf::Color( 255, 255, 255, 40 ) );

			m_cloud_sprites.push_back( cloud_sprite );
		}
	}

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
		if( m_options_window ) {
			on_options_reject();
		}
		else if( m_start_game_window ) {
			on_start_game_reject();
		}
		else {
			leave();
		}
	}
}

void MenuState::update( const sf::Time& delta ) {
	float seconds( delta.asSeconds() );

	// Move clouds.
	SpriteList::iterator cloud_iter( m_cloud_sprites.begin() );
	SpriteList::iterator cloud_iter_end( m_cloud_sprites.end() );

	for( ; cloud_iter != cloud_iter_end; ++cloud_iter ) {
		cloud_iter->move( seconds * 10.f, seconds * -5.f );

		if( cloud_iter->getGlobalBounds().left >= static_cast<float>( get_render_target().getSize().x ) ) {
			cloud_iter->move( -static_cast<float>( get_render_target().getSize().x ) - cloud_iter->getGlobalBounds().width, 0.f );
		}

		if( cloud_iter->getGlobalBounds().top + cloud_iter->getGlobalBounds().height <= 0.f ) {
			cloud_iter->move( 0.f, static_cast<float>( get_render_target().getSize().y ) + cloud_iter->getGlobalBounds().height );
		}
	}

	// Update desktop.
	m_desktop.Update( seconds );
}

void MenuState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.clear( sf::Color( 0x12, 0x34, 0x56 ) );

	// Clouds.
	SpriteList::const_iterator cloud_iter( m_cloud_sprites.begin() );
	SpriteList::const_iterator cloud_iter_end( m_cloud_sprites.end() );

	for( ; cloud_iter != cloud_iter_end; ++cloud_iter ) {
		window.draw( *cloud_iter );
	}

	// Render GUI.
	sfg::Renderer::Get().Display( window );

	window.display();
}

void MenuState::on_options_click() {
	if( m_options_window ) {
		return;
	}

	m_options_window = OptionsWindow::Create( get_shared().user_settings );
	m_options_window->SetRequisition( sf::Vector2f( 500.f, 0.f ) );
	m_desktop.Add( m_options_window );

	m_options_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ) / 2.f - m_options_window->GetAllocation().width / 2.f,
			static_cast<float>( get_render_target().getSize().y ) / 2.f - m_options_window->GetAllocation().height / 2.f
		)
	);

	sfg::DynamicPointerCast<OptionsWindow>( m_options_window )->OnAccept.Connect( &MenuState::on_options_accept, this );
	sfg::DynamicPointerCast<OptionsWindow>( m_options_window )->OnReject.Connect( &MenuState::on_options_reject, this );

	m_window->Show( false );
}

void MenuState::on_start_game_click() {
	if( m_start_game_window ) {
		return;
	}

	m_start_game_window = StartGameWindow::Create();
	m_desktop.Add( m_start_game_window );

	m_start_game_window->SetPosition(
		sf::Vector2f(
			static_cast<float>( get_render_target().getSize().x ) / 2.f - m_start_game_window->GetAllocation().width / 2.f,
			static_cast<float>( get_render_target().getSize().y ) / 2.f - m_start_game_window->GetAllocation().height / 2.f
		)
	);

	sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->OnAccept.Connect( &MenuState::on_start_game_accept, this );
	sfg::DynamicPointerCast<StartGameWindow>( m_start_game_window )->OnReject.Connect( &MenuState::on_start_game_reject, this );

	m_window->Show( false );

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

	m_desktop.Remove( m_options_window );
	m_options_window.reset();

	m_window->Show( true );
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
	m_desktop.Remove( m_options_window );
	m_options_window.reset();

	m_window->Show( true );
}

void MenuState::on_quit_click() {
	leave();
}

void MenuState::on_start_game_accept() {
	m_desktop.Remove( m_start_game_window );

	if( m_start_game_window->is_game_mode_selected() ) {
		// Prepare backend and session host.
		get_shared().account_manager.reset( new flex::AccountManager );
		get_shared().lock_facility.reset( new flex::LockFacility );
		get_shared().world.reset( new flex::World );

		get_shared().host.reset(
			new flex::SessionHost(
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
			m_window->Show( true );
		}
	}

	m_start_game_window.reset();
}

void MenuState::on_start_game_reject() {
	m_desktop.Remove( m_start_game_window );
	m_start_game_window.reset();

	m_window->Show( true );
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
