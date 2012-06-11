#include "MenuState.hpp"
#include "StateFactory.hpp"
#include "Shared.hpp"
#include "OptionsDocumentController.hpp"
#include "RocketRenderInterface.hpp"
#include "RocketSystemInterface.hpp"
#include "RocketEventDispatcher.hpp"
#include "RocketUtils.hpp"

#include <FlexWorld/Config.hpp>
#include <FlexWorld/GameModeDriver.hpp>

#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

static const bool SHOW_NOTICES = false;
static const float FADE_SPEED = 3000.0f;
static const float SLIDE_TARGET_X = 50.f;

MenuState::MenuState( sf::RenderWindow& target ) :
	State( target ),
	m_background_varray( sf::Quads, 4 ),
	m_rocket_context( nullptr ),
	m_options_document( nullptr )
{
}

void MenuState::init() {
	srand( static_cast<unsigned int>( time( nullptr ) ) );

	set_render_fps( 60 );

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

	// Set version string.
	{
		std::stringstream sstr;
		sstr
			<< static_cast<int>( flex::VERSION.get_major() ) << "."
			<< static_cast<int>( flex::VERSION.get_minor() ) << "."
			<< static_cast<int>( flex::VERSION.get_revision() ) << " "
			<< flex::VERSION_SUFFIX
		;

		Rocket::Core::Element* version_element = document->GetElementById( "version" );
		assert( version_element );
		version_element->SetInnerRML( sstr.str().c_str() );
	}

	document->RemoveReference();

	// Setup options document + controller.
	m_options_document = m_rocket_context->LoadDocument( (flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/options.rml" )).c_str() );
	m_options_controller.reset( new OptionsDocumentController( *m_options_document ) );

	m_options_controller->on_reject = std::bind( &MenuState::on_options_reject, this );
	m_options_controller->on_accept = std::bind( &MenuState::on_options_accept, this );

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
	m_options_controller.reset();

	m_options_document->RemoveReference();
	m_rocket_context->RemoveReference();

	Rocket::Core::ReleaseTextures();
	Rocket::Core::ReleaseCompiledGeometries();
}

void MenuState::handle_event( const sf::Event& event ) {
	if( event.type == sf::Event::Closed ) {
		leave();
	}

	// If options window is visible and waiting for a key/button, give event to
	// it and don't process it any further.
	if( m_options_document->IsVisible() && m_options_controller->is_waiting_for_press() ) {
		m_options_controller->handle_event( event );
		return;
	}

	RocketEventDispatcher::dispatch_event( event, *m_rocket_context );

	if( event.type == sf::Event::KeyPressed ) {
		if( event.key.code == sf::Keyboard::Escape ) {
			// If options window visible, close it.
			if( m_options_document->IsVisible() ) {
				on_options_reject();
			}
			else {
				// No GUI active, leave game. TODO: Confirmation dialog/couple with Quit button.
				on_quit_click();
			}
		}
		else if( event.key.code == sf::Keyboard::Return ) { // XXX
			on_insta_click();
		}
	}

}

void MenuState::update( const sf::Time& /*delta*/ ) {
	// Update rocket.
	m_rocket_context->Update();
}

void MenuState::render() const {
	sf::RenderWindow& window( get_render_target() );

	window.clear( sf::Color( 0x66, 0x9c, 0xff ) );

	{
		sf::RenderStates states;
		states.texture = &m_background_texture;

		window.draw( m_background_varray, states );
	}

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

void MenuState::on_options_accept() {
	// Get new settings.
	UserSettings new_settings = m_options_controller->get_user_settings();

	// Do we need to reinitialize the window?
	bool reinit_window =
		get_shared().user_settings.get_video_mode().width != new_settings.get_video_mode().width ||
		get_shared().user_settings.get_video_mode().height != new_settings.get_video_mode().height ||
		get_shared().user_settings.get_video_mode().bitsPerPixel != new_settings.get_video_mode().bitsPerPixel ||
		get_shared().user_settings.is_fullscreen_enabled() != new_settings.is_fullscreen_enabled()
	;

	// Remember old fullscreen state.
	bool was_fullscreen = get_shared().user_settings.is_fullscreen_enabled();

	// Apply user settings.
	get_shared().user_settings = new_settings;

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

	m_options_document->Show( Rocket::Core::ElementDocument::FOCUS );
	m_options_document->Hide();
}

void MenuState::on_options_reject() {
	m_options_document->Show( Rocket::Core::ElementDocument::FOCUS );
	m_options_document->Hide();
}

void MenuState::on_quit_click() {
	leave();
}

void MenuState::on_insta_click() {
	// TODO: Use 'start game' window.

	// Create IO service.
	get_shared().io_service.reset( new boost::asio::io_service );

	// Get selected game mode. TODO
	//const flex::GameMode& game_mode = m_start_game_window->get_selected_game_mode();
	std::ifstream in( (flex::ROOT_DATA_DIRECTORY + std::string( "modes/sandbox.yml" )).c_str() );
	std::stringstream buffer;

	buffer << in.rdbuf();
	flex::GameMode game_mode = flex::GameModeDriver::deserialize( buffer.str() );

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

	leave( StateFactory::create_connect_state( get_render_target() ) );
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
