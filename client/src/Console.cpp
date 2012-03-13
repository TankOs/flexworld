#include "Console.hpp"

Console::Ptr Console::Create() {
	Ptr console( new Console );

	// Layout.
	console->m_scrolled_window->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );
	console->m_scrolled_window->SetPlacement( sfg::ScrolledWindow::TOP_LEFT );
	console->m_scrolled_window->AddWithViewport( console->m_lines_box );

	sfg::Box::Ptr command_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.f ) );
	command_box->Pack( sfg::Label::Create( L"Command:" ), false );
	command_box->Pack( console->m_command_entry, true );
	command_box->Pack( console->m_ok_button, false );

	sfg::Box::Ptr vbox = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );
	vbox->Pack( console->m_scrolled_window, true );
	vbox->Pack( command_box, false );

	console->Add( vbox );

	// Init.
	console->SetTitle( L"Console" );

	console->add_message( "Console ready." );

	return console;
}


Console::Console() :
	sfg::Window( sfg::Window::Toplevel ),
	m_scrolled_window( sfg::ScrolledWindow::Create() ),
	m_lines_box( sfg::Box::Create( sfg::Box::VERTICAL, 5.f ) ),
	m_command_entry( sfg::Entry::Create() ),
	m_ok_button( sfg::Button::Create( L"OK" ) )
{
}

void Console::add_message( const sf::String& msg ) {
	sfg::Label::Ptr new_line( sfg::Label::Create( sf::String( "*** " ) + msg ) );
	sfg::Alignment::Ptr align( sfg::Alignment::Create() );

	align->SetScale( sf::Vector2f( 0, 0 ) );
	align->SetAlignment( sf::Vector2f( 0, 0 ) );
	align->Add( new_line );

	m_lines_box->Pack( align, false, false );

	OnMessageAdd();
}

std::size_t Console::get_num_messages() const {
	return m_lines_box->GetChildren().size();
}

const sf::String& Console::get_message( std::size_t index ) const {
	sfg::Alignment::PtrConst align = sfg::DynamicPointerCast<sfg::Alignment>( m_lines_box->GetChildren()[index] );
	sfg::Label::Ptr label = sfg::DynamicPointerCast<sfg::Label>( align->GetChildren()[0] );

	return label->GetText();
}
