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
		bool result = (e.what() == msg);

		if( !result ) {
			std::cout << "Expected: " << msg << std::endl;
			std::cout << "Thrown:   " << e.what() << std::endl;
		}

		return result;
	}

	std::string msg;
};

