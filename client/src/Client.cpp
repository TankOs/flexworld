#include "Client.hpp"
#include "IntroState.hpp"
#include "Shared.hpp"

#include <FlexWorld/Config.hpp>
#include <FlexWorld/Log.hpp>

#include <boost/filesystem.hpp>
#include <sstream>
#include <iostream>

Client::Client() {
	// Setup logger.
	#if defined( NDEBUG )
		flex::Log::Logger.set_min_level( flex::Log::INFO );
	#else
		flex::Log::Logger.set_min_level( flex::Log::DEBUG );
	#endif
}

void Client::run() {
	// Setup window.
	m_window.Create( sf::VideoMode::GetDesktopMode(), "", 0 );

	{
		std::stringstream sstr;
		sstr << "FlexWorld "
			<< static_cast<int>( flex::VERSION.get_major() ) << "."
			<< static_cast<int>( flex::VERSION.get_minor() ) << "."
			<< static_cast<int>( flex::VERSION.get_revision() ) << " "
			<< flex::VERSION_SUFFIX
		;

		m_window.SetTitle( sstr.str() );
	}

	// Make sure user's profile directory exists.
	if( !boost::filesystem::exists( UserSettings::get_profile_path() ) ) {
		// Try to create the directory.
		if( !boost::filesystem::create_directory( UserSettings::get_profile_path() ) ) {
			std::cerr << "Failed to create profile directory: " << UserSettings::get_profile_path() << std::endl;
			return;
		}
	}
	else {
		// Make sure profile path is a directory.
		if( !boost::filesystem::is_directory( UserSettings::get_profile_path() ) ) {
			std::cerr << "Profile path is not a directory: " << UserSettings::get_profile_path() << std::endl;
			return;
		}
	}

	// Try to load user settings if they exist.
	{
		bool set_defaults( false );

		if( boost::filesystem::exists( UserSettings::get_profile_path() + "/settings.yml" ) ) {
			if( !Shared::get().user_settings.load( UserSettings::get_profile_path() + "/settings.yml" ) ) {
				std::cerr << "Failed to load settings from " << UserSettings::get_profile_path() << "/settings.yml." << std::endl;
				set_defaults = true;
			}
		}
		else {
			// Settings file does not exist, set defaults.
			set_defaults = true;
		}

		if( set_defaults ) {
			std::cerr << "Using default values for settings." << std::endl;

			Controls& controls = Shared::get().user_settings.get_controls();

			controls.map_key( sf::Keyboard::W, Controls::WALK_FORWARD );
			controls.map_key( sf::Keyboard::S, Controls::WALK_BACKWARD );
			controls.map_key( sf::Keyboard::A, Controls::STRAFE_LEFT );
			controls.map_key( sf::Keyboard::D, Controls::STRAFE_RIGHT );
			controls.map_key( sf::Keyboard::Space, Controls::JUMP );
			controls.map_key( sf::Keyboard::LControl, Controls::CROUCH );
			controls.map_key( sf::Keyboard::E, Controls::USE );
			controls.map_key( sf::Keyboard::X, Controls::DROP );
			controls.map_key( sf::Keyboard::F, Controls::INVENTORY );
			controls.map_key( sf::Keyboard::T, Controls::CHAT );

			controls.map_button( sf::Mouse::Left, Controls::PRIMARY_ATTACK );
			controls.map_button( sf::Mouse::Right, Controls::SECONDARY_ATTACK );
		}
	}

	// Setup window.
	m_window.EnableVerticalSync( Shared::get().user_settings.is_vsync_enabled() );

	// Launch first state.
	State* state( new IntroState( m_window ) );

	while( state ) {
		State* next_state = state->run();

		delete state;
		state = next_state;
	}
}
