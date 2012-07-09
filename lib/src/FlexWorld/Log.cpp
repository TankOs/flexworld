#include <FlexWorld/Log.hpp>

#include <sstream>
#include <iomanip>
#include <ctime>
#include <cassert>

namespace fw {

Log Log::Logger;
const char Log::endl = '\n';

Log::Log() :
	m_filename( "out.log" ),
	m_next_level( DEBUG ),
	m_min_level( DEBUG ),
	m_file_logging( false ),
	m_console_logging( true )
{
}

void Log::set_log_filename( const std::string& filename ) {
	assert( !filename.empty() );
	m_filename = filename;
}

void Log::enable_file_logging( bool enable ) {
	m_file_logging = enable;
}

void Log::enable_console_logging( bool enable ) {
	m_console_logging = enable;
}

void Log::set_min_level( Level level ) {
	m_min_level = level;
}

Log& Log::operator()( Level level ) {
	m_next_level = level;

	if( m_next_level < m_min_level ) {
		return *this;
	}

	// Get current time.
	std::time_t time = std::time( 0 );
	std::tm* time_s = std::localtime( &time );

	std::stringstream intro;

	intro
		<< "["
		<< std::setfill( '0' ) << std::setw( 2 ) << time_s->tm_hour << ":"
		<< std::setfill( '0' ) << std::setw( 2 ) << time_s->tm_min << ":"
		<< std::setfill( '0' ) << std::setw( 2 ) << time_s->tm_sec
		<< "] "
		<< std::setfill( ' ' ) << std::setw( 11 )
	;

	if( level == DEBUG ) {
		intro << "DEBUG";
	}
	else if( level == INFO ) {
		intro << "INFO";
	}
	else if( level == WARNING ) {
		intro << "WARNING";
	}
	else if( level == ERR ) {
		intro << "ERROR";
	}
	else if( level == FATAL ) {
		intro << "***FATAL***";
	}

	intro << ": ";

	if( m_console_logging ) {
		std::cout << intro.str();
	}

	return *this;
}

}
