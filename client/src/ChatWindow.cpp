#include "ChatWindow.hpp"

#include <SFGUI/Box.hpp>

ChatWindow::Ptr ChatWindow::Create() {
	Ptr ptr( new ChatWindow );

	// Widgets.
	ptr->m_notebook = sfg::Notebook::Create();
	ptr->m_input_entry = sfg::Entry::Create();
	ptr->m_send_button = sfg::Button::Create( "Send" );

	// Layout.
	sfg::Box::Ptr send_box = sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f );
	send_box->Pack( ptr->m_input_entry, true, true );
	send_box->Pack( ptr->m_send_button, false, true );

	sfg::Box::Ptr vbox = sfg::Box::Create( sfg::Box::VERTICAL, 5.0f );
	vbox->Pack( ptr->m_notebook, true, true );
	vbox->Pack( send_box, false, true );

	ptr->Add( vbox );

	// Init.
	ptr->SetTitle( L"Chat" );
	ptr->CreateChannel( "Status" );

	return ptr;
}

ChatWindow::ChatWindow() :
	sfg::Window( sfg::Window::TOPLEVEL )
{
}

void ChatWindow::CreateChannel( const std::string& name ) {
	if( FindChannel( name ) != nullptr ) {
		return;
	}

	Channel chan;

	// Widgets.
	chan.name = name;
	chan.scrolled_window = sfg::ScrolledWindow::Create();

	// Init.
	chan.scrolled_window->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );

	m_channels.push_back( chan );

	m_notebook->AppendPage( chan.scrolled_window, sfg::Label::Create( name ) );

	Refresh();
}

ChatWindow::Channel* ChatWindow::FindChannel( const std::string& name ) {
	for( std::size_t idx = 0; idx < m_channels.size(); ++idx ) {
		if( m_channels[idx].name == name ) {
			return &m_channels[idx];
		}
	}

	return nullptr;
}

void ChatWindow::FocusEntry() {
	m_input_entry->GrabFocus();
}

void ChatWindow::ClearEntry() {
	m_input_entry->SetText( "" );
}
