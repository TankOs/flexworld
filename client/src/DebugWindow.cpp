#include "DebugWindow.hpp"

#include <SFGUI/Button.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/ScrolledWindow.hpp>
#include <SFGUI/Context.hpp>
#include <cassert>

DebugWindow::Ptr DebugWindow::Create() {
	Ptr window( new DebugWindow );

	window->SetRequisition( sf::Vector2f( 200, 400 ) );

	// Widgets.
	typedef std::pair<sfg::Button::Ptr, std::string> ButtonIdPair;
	typedef std::vector<ButtonIdPair> ButtonIdPairArray;

	ButtonIdPairArray buttons;

	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Backpack" ), "fw.containers/backpack" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Dwarf" ), "fw.base.human/dwarf_male" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Grass" ), "fw.base.nature/grass" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Horse" ), "fw.animals/horse" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Pillar (bottom)" ), "fw.rome.pillar/bottom" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Pillar (middle)" ), "fw.rome.pillar/middle" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Pillar (top)" ), "fw.rome.pillar/top" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Shovel" ), "fw.tools/shovel" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Stone mural" ), "fw.building/stone_mural" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Stone" ), "fw.base.nature/stone" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Wooden fence" ), "fw.building/wooden_fence" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Wooden planks" ), "fw.base.nature/wooden_planks" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Wooden planks/grass (EW)" ), "fw.base.nature/wooden_planks_grass_we" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Wooden planks/grass (WE)" ), "fw.base.nature/wooden_planks_grass_ew" ) );
	buttons.push_back( ButtonIdPair( sfg::Button::Create( "Wooden stair" ), "fw.building/wooden_stair" ) );

	// Layout.
	sfg::Box::Ptr buttons_box = sfg::Box::Create( sfg::Box::VERTICAL, 5.0f );

	for( std::size_t idx = 0; idx < buttons.size(); ++idx ) {
		buttons_box->Pack( buttons[idx].first, false, true );

		buttons[idx].first->OnLeftClick.Connect( &DebugWindow::HandleClassClick, &*window );
		window->m_buttons[buttons[idx].first] = buttons[idx].second;
	}

	sfg::ScrolledWindow::Ptr scrolled = sfg::ScrolledWindow::Create();
	scrolled->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );

	scrolled->AddWithViewport( buttons_box );

	window->Add( scrolled );

	// Init.
	window->SetTitle( "Debug window" );

	return window;
}

DebugWindow::DebugWindow() :
	sfg::Window( sfg::Window::TOPLEVEL )
{
}

const std::string& DebugWindow::GetCurrentID() const {
	return m_current_id;
}

void DebugWindow::HandleClassClick() {
	sfg::Button::Ptr button = sfg::DynamicPointerCast<sfg::Button>( sfg::Context::Get().GetActiveWidget() );
	assert( button );

	m_current_id = m_buttons[button];

	OnClassIDChange();
}
