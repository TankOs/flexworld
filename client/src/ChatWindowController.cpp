#include "ChatWindowController.hpp"

#include <Rocket/Core.h>

ChatWindowController::ChatWindowController( Rocket::Core::Element* root ) :
	m_root( root )
{
	m_root->AddReference();
}

ChatWindowController::~ChatWindowController() {
	m_root->RemoveReference();
}
