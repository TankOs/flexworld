#pragma once

#include <stdexcept>

#define FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( NAME ) class NAME : public std::runtime_error { public: NAME( const std::string& msg ) : std::runtime_error( msg ) {} }
