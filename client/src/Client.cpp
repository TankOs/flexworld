#include "Client.hpp"
#include "IntroState.hpp"

#include <FlexWorld/Config.hpp>
#include <sstream>

Client::Client() {
}

void Client::run() {
	// Setup window.
	m_window.Create( sf::VideoMode::GetDesktopMode(), "", sf::Style::Fullscreen );

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
