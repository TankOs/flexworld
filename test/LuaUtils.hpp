#pragma once

#include <string>

namespace Diluculum {
class LuaState;
}

bool check_error( const std::string& error, const std::string& code, Diluculum::LuaState& state );
