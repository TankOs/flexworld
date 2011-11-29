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
	if( boost::filesystem::exists( UserSettings::get_profile_path() + "/settings.yml" ) ) {
		if( !Shared::get().get_user_settings().load( UserSettings::get_profile_path() + "/settings.yml" ) ) {
			std::cerr << "Failed to load settings from " << UserSettings::get_profile_path() << "/settings.yml." << std::endl;
			std::cerr << "Starting with default user settings." << std::endl;
		}
	}
	else {
			std::cerr << "No user settings saved, starting with defaults." << std::endl;
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
