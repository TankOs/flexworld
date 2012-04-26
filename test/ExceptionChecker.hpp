#pragma once

#include <string>
#include <iostream>

template <class T>
struct ExceptionChecker {
	ExceptionChecker( const std::string& msg_ ) :
		msg( msg_ )
	{
	}

	bool operator()( const T& e ) {
		std::string thrown_msg = e.what();
		std::size_t colon_pos = thrown_msg.find( ':' );

		if( colon_pos != std::string::npos ) {
			// Skip colon.
			++colon_pos;

			// Skip whitespace.
			while( colon_pos + 1 < thrown_msg.size() && thrown_msg[colon_pos] == ' ' ) {
				++colon_pos;
			}

			thrown_msg = thrown_msg.substr( colon_pos );
		}

		bool result = (thrown_msg == msg);

		if( !result ) {
			std::cout << "Expected: " << msg << std::endl;
			std::cout << "Thrown:   " << thrown_msg << std::endl;
		}

		return result;
	}

	std::string msg;
};

