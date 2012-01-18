#include <cstdint>

#include "StartGameWindow.hpp"

StartGameWindow::StartGameWindow() :
	Window( Window::Toplevel )
{
}

StartGameWindow::Ptr StartGameWindow::Create() {
	Ptr window( new StartGameWindow );

	// Widgets.
	window->SetTitle( L"Start game" );

	sfg::Button::Ptr ok_button( sfg::Button::Create( L"Play!" ) );
	sfg::Button::Ptr cancel_button( sfg::Button::Create( L"Cancel" ) );

	window->m_game_mode_combo = sfg::ComboBox::Create();
	window->m_password_entry = sfg::Entry::Create();
	window->m_password_entry->HideText( '*' );
	window->m_public_check = sfg::CheckButton::Create( L"Others can join this game*" );
	window->m_max_players_scale = sfg::Scale::Create( 1.f, 16.f, 1.f );
	window->m_max_players_label = sfg::Label::Create( L"1" );
	window->m_max_players_label->SetRequisition( sf::Vector2f( 25.f, 0.f ) );

	sfg::Label::Ptr inet_label( sfg::Label::Create( L"* Internet connection required." ) );
	inet_label->SetClass( "info" );

	// Layout.
	uint32_t row_index( 0 );
	sfg::Table::Ptr table( sfg::Table::Create() );
	table->SetRowSpacings( 10.f );
	table->SetColumnSpacings( 10.f );

	table->Attach( sfg::Label::Create( L"Game mode:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	table->Attach( window->m_game_mode_combo, sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	sfg::Frame::Ptr general_frame = sfg::Frame::Create( L"General" );
	general_frame->Add( table );

	// Multiplayer options.
	sfg::Box::Ptr max_players_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 10.f ) );
	max_players_box->Pack( window->m_max_players_scale, true );
	max_players_box->Pack( window->m_max_players_label, false );

	sfg::Table::Ptr multiplayer_table = sfg::Table::Create();
	multiplayer_table->SetRowSpacings( 10.f );
	multiplayer_table->SetColumnSpacings( 10.f );

	row_index = 0;
	++row_index;
	multiplayer_table->Attach( sfg::Label::Create( L"Password:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	multiplayer_table->Attach( sfg::Label::Create( L"Max. players:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	multiplayer_table->Attach( window->m_public_check, sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	multiplayer_table->Attach( window->m_password_entry, sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	multiplayer_table->Attach( max_players_box, sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	multiplayer_table->Attach( inet_label, sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), 0, sfg::Table::FILL );

	sfg::Frame::Ptr multiplayer_frame = sfg::Frame::Create( L"Multiplayer" );
	multiplayer_frame->Add( multiplayer_table );

	// Dialog buttons.
	sfg::Box::Ptr bottom_button_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.f ) );
	bottom_button_box->Pack( cancel_button, false );
	bottom_button_box->Pack( ok_button, false );

	sfg::Alignment::Ptr alignment( sfg::Alignment::Create() );
	alignment->SetAlignment( sf::Vector2f( 1.f, 1.f ) );
	alignment->SetScale( sf::Vector2f( 0.f, 0.f ) );
	alignment->Add( bottom_button_box );

	sfg::Box::Ptr content_box( sfg::Box::Create( sfg::Box::VERTICAL, 10.f ) );
	content_box->Pack( general_frame, false );
	content_box->Pack( multiplayer_frame, false );
	content_box->Pack( alignment, true );

	window->Add( content_box );

	// Signals.
	ok_button->OnClick.Connect( &StartGameWindow::on_ok_click, &*window );
	cancel_button->OnClick.Connect( &StartGameWindow::on_cancel_click, &*window );

	window->m_max_players_scale->GetAdjustment()->OnChange.Connect( &StartGameWindow::on_max_players_change, &*window );

	return window;
}

void StartGameWindow::on_ok_click() {
	OnAccept();
}

void StartGameWindow::on_cancel_click() {
	OnReject();
}

void StartGameWindow::on_max_players_change() {
	std::stringstream sstr;

	sstr << static_cast<int>( m_max_players_scale->GetValue() );
	m_max_players_label->SetText( sstr.str() );
}

void StartGameWindow::add_game_mode( const flex::GameMode& game_mode ) {
	std::stringstream sstr;

	sstr
		<< game_mode.get_name()
		<< " ("
		<< game_mode.get_version().get_major() << "."
		<< game_mode.get_version().get_minor() << "."
		<< game_mode.get_version().get_revision()
		<< ")"
	;

	m_game_mode_combo->AppendItem( sstr.str() );

	// Select the first added game mode.
	if( m_game_mode_combo->GetItemCount() == 1 ) {
		m_game_mode_combo->SelectItem( 0 );
	}

	// Store for returning it later.
	m_game_modes.push_back( game_mode );
}
