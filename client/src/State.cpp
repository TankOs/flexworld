#include "State.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

State::State( sf::RenderWindow& target ) :
	m_render_target( target ),
	m_render_fps( 0 ),
	m_run( false ),
	m_next_state( nullptr )
{
	set_render_fps( 60 );
}

State::~State() {
}

State* State::run() {
	// Skip if already running.
	if( m_run ) {
		return nullptr;
	}

	m_run = true;
	sf::Clock fps_timer;
	sf::Clock update_timer;
	sf::Clock frame_timer;

	sf::Event event;

	unsigned int current_render_fps = 0;

	init();

	while( m_run ) {
		// Count frame time.
		frame_timer.restart();

		// Process events.
		while( m_render_target.pollEvent( event ) ) {
			// Check for F12 (screenshot).
			if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F12 ) {
				// Construct base filename with date and time.
				boost::posix_time::ptime now( boost::posix_time::second_clock::local_time() );

				std::stringstream basename;
				basename
					<< "flexworld_"
					<< std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( now.date().year() ) << "-"
					<< std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( now.date().month() ) << "-"
					<< std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( now.date().day() )
					<< "_"
					<< std::setw( 2 ) << std::setfill( '0' ) << now.time_of_day().hours() << "-"
					<< std::setw( 2 ) << std::setfill( '0' ) << now.time_of_day().minutes() << "-"
					<< std::setw( 2 ) << std::setfill( '0' ) << now.time_of_day().seconds()
					<< "-"
				;

				// Find next free screenshot ID.
				uint32_t id( 0 );
				std::ifstream in;
				std::string filename;

				do {
					std::stringstream sstr;
					sstr << basename.str() << id << ".png";
					filename = sstr.str();

					in.close();
					in.open( filename.c_str() );

					++id;
				} while( in.is_open() );

				// Save.
				sf::Image screenshot( get_render_target().capture() );
				screenshot.saveToFile( filename );
			}

			// Give event to state.
			handle_event( event );
		}

		// Logic.
		update( update_timer.restart() );

		// Rendering.
		render();
		++current_render_fps;

		// Update FPS.
		if( fps_timer.getElapsedTime() >= sf::microseconds( 1000000 ) ) {
			m_render_fps = current_render_fps;
			current_render_fps = 0;
			fps_timer.restart();
		}

		// Wait if FPS is locked.
		if( m_render_interval != sf::Time::Zero ) {
			sf::Time wait_time = m_render_interval - frame_timer.getElapsedTime();

			if( wait_time > sf::Time::Zero ) {
				sf::sleep( wait_time );
			}
		}
	}

	cleanup();

	return m_next_state;
}

void State::set_render_fps( uint16_t fps ) {
	if( fps == 0 ) {
		m_render_interval = sf::Time::Zero;
	}
	else {
		m_render_interval = sf::microseconds( 1000000 / fps );
	}
}

void State::leave( State* state ) {
	if( m_next_state ) {
		delete m_next_state;
	}

	m_next_state = state;
	m_run = false;
}

sf::RenderWindow& State::get_render_target() const {
	return m_render_target;
}

unsigned int State::get_render_fps() const {
	return m_render_fps;
}
