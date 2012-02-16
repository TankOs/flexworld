#include "State.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

State::State( sf::RenderWindow& target ) :
	m_render_target( target ),
	m_render_fps( 0 ),
	m_logic_fps( 0 ),
	m_run( false ),
	m_next_state( nullptr )
{
	set_logic_fps( 200 );
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
	sf::Clock clock;

	sf::Time logic_elapsed;
	sf::Time render_elapsed;
	sf::Time fps_elapsed;
	sf::Time elapsed;
	sf::Event event;

	unsigned int current_render_fps = 0;
	unsigned int current_logic_fps = 0;

	init();

	while( m_run ) {
		// Process events.
		while( m_render_target.PollEvent( event ) ) {
			// Check for F12 (screenshot).
			if( event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::F12 ) {
				// Find next free screenshot ID.
				uint32_t id( 0 );
				std::ifstream in;
				std::string filename;

				do {
					std::stringstream sstr;
					sstr << "screenshot" << std::setfill( '0' ) << std::setw( 5 ) << id << ".png";
					filename = sstr.str();

					in.close();
					in.open( filename.c_str() );

					++id;
				} while( in.is_open() );

				// Save.
				sf::Image screenshot( get_render_target().Capture() );
				screenshot.SaveToFile( filename );
			}
			else {
				handle_event( event );
			}
		}

		elapsed = clock.Restart();

		logic_elapsed += elapsed;
		render_elapsed += elapsed;
		fps_elapsed += elapsed;

		// If interval for logic AND rendering not expired, save the CPU the trouble.
		if( logic_elapsed < m_logic_interval && render_elapsed < m_render_interval ) {
			sf::Time wait_time( std::min( m_logic_interval - logic_elapsed, m_render_interval - render_elapsed ) );

			sf::Sleep( wait_time );

			logic_elapsed += wait_time;
			render_elapsed += wait_time;
			fps_elapsed += wait_time;

			// We don't want the wait time to be added to the elapsed time again later.
			clock.Restart();
		}

		// Logics. Run until all remaining slices have been updated. In case longer
		// breaks happen, this loop will try to close up.
		while( logic_elapsed >= m_logic_interval ) {
			update( m_logic_interval );
			logic_elapsed -= m_logic_interval;
			++current_logic_fps;

			if( !m_run ) {
				break;
			}
		}

		// Rendering.
		if( render_elapsed >= m_render_interval ) {
			render();
			render_elapsed = sf::Time::Zero;
			++current_render_fps;
		}

		// Update FPS.
		if( fps_elapsed >= sf::Microseconds( 1000000 ) ) {
			m_render_fps = current_render_fps;
			m_logic_fps = current_logic_fps;

			current_render_fps = 0;
			current_logic_fps = 0;

			fps_elapsed = sf::Time::Zero;
		}
	}

	cleanup();

	return m_next_state;
}

void State::set_logic_fps( uint16_t fps ) {
	assert( fps > 0 );
	m_logic_interval = sf::Microseconds( 1000000 / fps );
}

void State::set_render_fps( uint16_t fps ) {
	assert( fps > 0 );
	m_render_interval = sf::Microseconds( 1000000 / fps );
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

unsigned int State::get_logic_fps() const {
	return m_logic_fps;
}
