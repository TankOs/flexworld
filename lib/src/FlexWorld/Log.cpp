#include <FlexWorld/Log.hpp>

#include <iomanip>
#include <ctime>
#include <cassert>

namespace flex {

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

	if( m_console_logging ) {
		std::cout
			<< "["
			<< std::setfill( '0' ) << std::setw( 2 ) << time_s->tm_hour << ":"
			<< std::setfill( '0' ) << std::setw( 2 ) << time_s->tm_min << ":"
			<< std::setfill( '0' ) << std::setw( 2 ) << time_s->tm_sec
			<< "] "
		;
	}

	return *this;
}

}
