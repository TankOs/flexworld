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

	// Signals.
	ptr->m_send_button->OnClick.Connect( &ChatWindow::OnSendButtonClick, &(*ptr) );
	ptr->m_input_entry->OnKeyPress.Connect( &ChatWindow::OnInputEntryKeyPress, &(*ptr) );

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

const sf::String& ChatWindow::GetMessage() const {
	return m_message;
}

void ChatWindow::OnSendButtonClick() {
	// Check for valid input.
	bool input_valid = true;

	if( m_input_entry->GetText().getSize() < 1 ) {
		input_valid = false;
	}
	else {
		// Check if only spaces were typed.
		bool char_found = false;

		for( std::size_t idx = 0; idx < m_input_entry->GetText().getSize(); ++idx ) {
			if( m_input_entry->GetText()[idx] != ' ' ) {
				char_found = true;
				break;
			}
		}

		input_valid = char_found;
	}

	if( !input_valid ) {
		return;
	}

	m_message = m_input_entry->GetText();
	ClearEntry();

	OnMessageReady();
}

void ChatWindow::OnInputEntryKeyPress() {
	// If input has focus and enter is pressed, send message.
	if( !m_input_entry->HasFocus() || !sf::Keyboard::isKeyPressed( sf::Keyboard::Return ) ) {
		return;
	}

	OnSendButtonClick();
}

std::size_t ChatWindow::GetNumChannels() const {
	return m_channels.size();
}

const std::string& ChatWindow::GetChannelName( std::size_t index ) const {
	assert( index < m_channels.size() );

	return m_channels[index].name;
}

std::size_t ChatWindow::GetActiveChannel() const {
	return static_cast<std::size_t>( m_notebook->GetCurrentPage() );
}
