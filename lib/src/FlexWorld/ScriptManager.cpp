#include <FlexWorld/ScriptManager.hpp>

#include <Diluculum/LuaState.hpp>

namespace flex {

ScriptManager::ScriptManager() :
	m_state( new Diluculum::LuaState )
{
	// Prepare "flex namespace".
	(*m_state)["flex"] = Diluculum::EmptyTable;
}

ScriptManager::~ScriptManager() {
	delete m_state;
}

}
