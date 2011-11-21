#include "OptionsWindow.hpp"

OptionsWindow::OptionsWindow() :
	Window()
{
}

OptionsWindow::Ptr OptionsWindow::Create() {
	Ptr window( new OptionsWindow );

	// Widgets.
	window->SetBorderWidth( 10.f );
	window->SetTitle( L"Options" );

	sfg::Button::Ptr account_button( sfg::Button::Create( L"Account" ) );
	sfg::Button::Ptr controls_button( sfg::Button::Create( L"Controls" ) );
	sfg::Button::Ptr graphics_button( sfg::Button::Create( L"Video" ) );
	sfg::Button::Ptr audio_button( sfg::Button::Create( L"Audio" ) );

	sfg::Button::Ptr ok_button( sfg::Button::Create( L"OK" ) );
	sfg::Button::Ptr cancel_button( sfg::Button::Create( L"Cancel" ) );

	// Account.
	window->m_username_entry = sfg::Entry::Create();
	window->m_serial_entry = sfg::Entry::Create();

	// Layout.
	sfg::Box::Ptr top_button_box( sfg::Box::Create( sfg::Box::Horizontal, 5.f ) );
	top_button_box->Pack( account_button, true );
	top_button_box->Pack( controls_button, true );
	top_button_box->Pack( graphics_button, true );
	top_button_box->Pack( audio_button, true );

	// Account.
	sfg::Table::Ptr acc_table( sfg::Table::Create() );
	acc_table->SetRowSpacings( 10.f );
	acc_table->SetColumnSpacings( 10.f );
	acc_table->Attach( sfg::Label::Create( L"Username:" ), sf::Rect<sf::Uint32>( 0, 0, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	acc_table->Attach( window->m_username_entry, sf::Rect<sf::Uint32>( 1, 0, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	acc_table->Attach( sfg::Label::Create( L"Serial:" ), sf::Rect<sf::Uint32>( 0, 1, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	acc_table->Attach( window->m_serial_entry, sf::Rect<sf::Uint32>( 1, 1, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	window->m_account_page_box = sfg::Box::Create( sfg::Box::Vertical, 5.f );
	window->m_account_page_box->Pack( acc_table, false );

	// Controls.
	sfg::Table::Ptr controls_table( sfg::Table::Create() );
	controls_table->SetRowSpacings( 10.f );
	controls_table->SetColumnSpacings( 10.f );

	uint32_t row_index( 0 );
	controls_table->Attach( sfg::Label::Create( L"Forward:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Backward:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Strafe left:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Strafe right:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Jump:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Crouch:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	controls_table->Attach( sfg::Label::Create( L"Use/Take:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Drop:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Primary attack:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Secondary attack:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Inventory:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Chat:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	controls_table->Attach( sfg::Button::Create( L"W" ), sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"S" ), sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"A" ), sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"D" ), sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"Space" ), sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"Left ctrl" ), sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	row_index = 0;
	controls_table->Attach( sfg::Button::Create( L"E" ), sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"X" ), sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"Left mouse" ), sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"Right mouse" ), sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"F" ), sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( sfg::Button::Create( L"T" ), sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	window->m_controls_page_box = sfg::Box::Create( sfg::Box::Vertical, 5.f );
	window->m_controls_page_box->Pack( controls_table, false );

	// ---
	sfg::Box::Ptr bottom_button_box( sfg::Box::Create( sfg::Box::Horizontal, 5.f ) );
	bottom_button_box->Pack( cancel_button, false );
	bottom_button_box->Pack( ok_button, false );

	sfg::Box::Ptr content_box( sfg::Box::Create( sfg::Box::Vertical, 10.f ) );
	content_box->Pack( top_button_box, false );
	content_box->Pack( window->m_account_page_box, true );
	content_box->Pack( window->m_controls_page_box, true );
	content_box->Pack( bottom_button_box, false );

	window->Add( content_box );

	window->show_page( window->m_account_page_box );

	// Signals.
	account_button->OnClick.Connect( &OptionsWindow::on_account_click, &*window );
	controls_button->OnClick.Connect( &OptionsWindow::on_controls_click, &*window );

	ok_button->OnClick.Connect( &OptionsWindow::on_ok_click, &*window );
	cancel_button->OnClick.Connect( &OptionsWindow::on_cancel_click, &*window );

	return window;
}

void OptionsWindow::show_page( sfg::Widget::PtrConst page ) {
	m_account_page_box->Show( page == m_account_page_box );
	m_controls_page_box->Show( page == m_controls_page_box );
}

void OptionsWindow::on_account_click() {
	show_page( m_account_page_box );
}

void OptionsWindow::on_controls_click() {
	show_page( m_controls_page_box );
}

void OptionsWindow::on_ok_click() {
	OnAccept();
}

void OptionsWindow::on_cancel_click() {
	OnReject();
}
