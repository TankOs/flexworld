#pragma once

#include <string>
#include <iostream>
#include <typeinfo>

namespace flex {

/** Simply logging utility class.
 * Console logging is enabled by default.
 */
class Log {
	public:
		/** Log level.
		 */
		enum Level {
			DEBUG = 0,
			INFO,
			WARNING,
			ERROR,
			FATAL
		};

		/** Ctor.
		 */
		Log();

		/** Set log filename, i.e. enables logging to file.
		 * Defaults to "out.log".
		 * @param filename Filename.
		 */
		void set_log_filename( const std::string& filename );

		/** Enable or disable file logging.
		 * @param enable true to enable.
		 */
		void enable_file_logging( bool enable = true );

		/** Enable or disable console logging.
		 * @param enable true to enable.
		 */
		void enable_console_logging( bool enable = true );

		/** Set minimum level of messages to show.
		 * @param level Level.
		 */
		void set_min_level( Level level );

		/** Prepare next message's level.
		 * You can use this to quickly output something with a specific log level, e.g. Log( DEBUG ) << "Foobar" << std::endl.
		 * @param level Level.
		 * @return *this.
		 */
		Log& operator()( Level level = INFO );

		/** Log something.
		 * @param data Data.
		 * @return *this.
		 */
		template <class T>
		Log& operator<<( const T& data );

		static Log Logger; ///< Simple predefined logger with default values.
		static const char endl; ///< End of line (newline).

	private:
		std::string m_filename;
		Level m_next_level;
		Level m_min_level;
		bool m_file_logging;
		bool m_console_logging;
};

}

#include "Log.inl"
