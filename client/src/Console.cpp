#include "Console.hpp"

Console::Ptr Console::Create() {
	Ptr console( new Console );
	return console;
}


Console::Console() :
	sfg::Window( sfg::Window::Toplevel ),
	m_scrolled_window( sfg::ScrolledWindow::Create() ),
	m_lines_box( sfg::Box::Create( sfg::Box::VERTICAL, 5.f ) ),
	m_command_entry( sfg::Entry::Create() ),
	m_ok_button( sfg::Button::Create( L"OK" ) )
{
	// Layout.
	m_scrolled_window->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );
	m_scrolled_window->SetPlacement( sfg::ScrolledWindow::TOP_LEFT );
	m_scrolled_window->AddWithViewport( m_lines_box );

	sfg::Box::Ptr command_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.f ) );
	command_box->Pack( sfg::Label::Create( L"Command:" ), false );
	command_box->Pack( m_command_entry, true );
	command_box->Pack( m_ok_button, false );

	sfg::Box::Ptr vbox = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );
	vbox->Pack( m_scrolled_window, true );
	vbox->Pack( command_box, false );

	Add( vbox );

	// Init.
	SetTitle( L"Console" );

	add_message( "Console ready." );
}

void Console::add_message( const sf::String& msg ) {
	sfg::Label::Ptr new_line( sfg::Label::Create( sf::String( "*** " ) + msg ) );
	sfg::Alignment::Ptr align( sfg::Alignment::Create() );

	align->SetScale( sf::Vector2f( 0, 0 ) );
	align->SetAlignment( sf::Vector2f( 0, 0 ) );
	align->Add( new_line );

	m_lines_box->Pack( align, false, false );
}
