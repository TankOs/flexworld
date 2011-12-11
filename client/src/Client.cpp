#include "Client.hpp"
#include "IntroState.hpp"
#include "Shared.hpp"

#include <FlexWorld/Config.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <iostream>

Client::Client() {
}

void Client::run() {
	// Setup window.
	m_window.Create( sf::VideoMode( 1280, 720 ), "", sf::Style::Titlebar | sf::Style::Close );
	//m_window.Create( sf::VideoMode::GetDesktopMode(), "", sf::Style::Fullscreen );

	{
		std::stringstream sstr;
		sstr << "FlexWorld "
			<< static_cast<int>( flex::VERSION_MAJOR ) << "."
			<< static_cast<int>( flex::VERSION_MINOR ) << "."
			<< static_cast<int>( flex::VERSION_REVISION ) << " "
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
			if( !Shared::get().get_user_settings().load( UserSettings::get_profile_path() + "/settings.yml" ) ) {
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

			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::W, Controls::WALK_FORWARD );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::S, Controls::WALK_BACKWARD );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::A, Controls::STRAFE_LEFT );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::D, Controls::STRAFE_RIGHT );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::Space, Controls::JUMP );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::LControl, Controls::CROUCH );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::E, Controls::USE );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::X, Controls::DROP );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::F, Controls::INVENTORY );
			Shared::get().get_user_settings().get_controls().map_key( sf::Keyboard::T, Controls::CHAT );

			Shared::get().get_user_settings().get_controls().map_button( sf::Mouse::Left, Controls::PRIMARY_ATTACK );
			Shared::get().get_user_settings().get_controls().map_button( sf::Mouse::Right, Controls::SECONDARY_ATTACK );
		}
	}

	// Launch first state.
	State* state( new IntroState( m_window ) );

	while( state ) {
		State* next_state( state->run() );

		delete state;
		state = nullptr;

		if( next_state ) {
			state = next_state;
		}
	}
}
