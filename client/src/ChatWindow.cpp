#include "ChatWindow.hpp"

#include <SFGUI/Alignment.hpp>

ChatWindow::Ptr ChatWindow::Create() {
	Ptr ptr( new ChatWindow );

	// Widgets.
	ptr->m_notebook = sfg::Notebook::Create();
	ptr->m_input_entry = sfg::Entry::Create();
	ptr->m_send_button = sfg::Button::Create( "Send" );
	ptr->m_clear_button = sfg::Button::Create( "Clear" );
	ptr->m_join_button = sfg::Button::Create( "Join" );
	ptr->m_close_button = sfg::Button::Create( "Close" );

	// Layout.
	sfg::Box::Ptr send_box = sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f );
	send_box->Pack( ptr->m_input_entry, true, true );
	send_box->Pack( ptr->m_send_button, false, true );

	sfg::Box::Ptr buttons_vbox = sfg::Box::Create( sfg::Box::VERTICAL, 5.0f );
	buttons_vbox->Pack( ptr->m_close_button, false, true );
	buttons_vbox->Pack( ptr->m_join_button, false, true );
	buttons_vbox->Pack( ptr->m_clear_button, false, true );

	sfg::Box::Ptr vbox = sfg::Box::Create( sfg::Box::VERTICAL, 5.0f );
	vbox->Pack( ptr->m_notebook, true, true );
	vbox->Pack( send_box, false, true );

	sfg::Box::Ptr hbox = sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f );
	hbox->Pack( vbox, true, true );
	hbox->Pack( buttons_vbox, false, true );

	ptr->Add( hbox );

	// Signals.
	ptr->m_send_button->GetSignal( sfg::Button::OnLeftClick ).Connect( &ChatWindow::OnSendButtonClick, &(*ptr) );
	ptr->m_input_entry->GetSignal( sfg::Entry::OnKeyPress ).Connect( &ChatWindow::OnInputEntryKeyPress, &(*ptr) );
	ptr->m_close_button->GetSignal( sfg::Button::OnLeftClick ).Connect( &ChatWindow::OnCloseButtonClick, &(*ptr) );

	// Init.
	ptr->SetTitle( L"Chat" );
	ptr->CreateChannel( "Status" );

	return ptr;
}

ChatWindow::ChatWindow() :
	sfg::Window( sfg::Window::TOPLEVEL )
{
}

void ChatWindow::CreateChannel( const sf::String& name ) {
	if( FindChannel( name ) != nullptr ) {
		return;
	}

	Channel chan;

	// Widgets.
	chan.name = name;
	chan.scrolled_window = sfg::ScrolledWindow::Create();
	chan.vbox = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );

	// Layout.
	chan.scrolled_window->AddWithViewport( chan.vbox );

	// Init.
	chan.scrolled_window->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );

	m_channels.push_back( chan );

	// Tab widget.
	sfg::Label::Ptr tab_label = sfg::Label::Create( name );
	sfg::Button::Ptr close_button = sfg::Button::Create( "X" );
	close_button->SetClass( "small" );

	sfg::Box::Ptr tab_box = sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f );
	tab_box->Pack( tab_label, true, true );
	tab_box->Pack( close_button, false, false );

	m_notebook->AppendPage( chan.scrolled_window, tab_box );

	Refresh();

	// FIXME Workaround SFGUI notebook bug.
	SetAllocation( sf::FloatRect( GetAllocation().left, GetAllocation().top, GetAllocation().width + 1, GetAllocation().height ) );
	SetAllocation( sf::FloatRect( GetAllocation().left, GetAllocation().top, GetAllocation().width - 1, GetAllocation().height ) );
}

ChatWindow::Channel* ChatWindow::FindChannel( const sf::String& name ) {
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

const sf::String& ChatWindow::GetChannelName( std::size_t index ) const {
	assert( index < m_channels.size() );

	return m_channels[index].name;
}

std::size_t ChatWindow::GetActiveChannel() const {
	return static_cast<std::size_t>( m_notebook->GetCurrentPage() );
}

void ChatWindow::AddMessage( const sf::String& message, const sf::String& channel ) {
	// Find channel.
	Channel* chan_ptr = FindChannel( channel );

	// Create if it doesn't exist.
	if( chan_ptr == nullptr ) {
		CreateChannel( channel );
	}

	chan_ptr = FindChannel( channel );
	assert( chan_ptr != nullptr );

	// Add message.
	sfg::Label::Ptr label = sfg::Label::Create( message );

	sfg::Alignment::Ptr alignment = sfg::Alignment::Create();
	alignment->SetScale( sf::Vector2f( 0, 1 ) );
	alignment->Add( label );

	chan_ptr->vbox->Pack( alignment, false, true );
	chan_ptr->scrolled_window->Refresh();

	// Scroll down.
	sfg::Adjustment::Ptr new_adjustment = sfg::Adjustment::Create();
	*new_adjustment = *chan_ptr->scrolled_window->GetVerticalAdjustment();

	new_adjustment->SetValue( new_adjustment->GetUpper() + 9999 );

	// Set new adjustment for scrolled window AND viewport(!).
	chan_ptr->scrolled_window->SetVerticalAdjustment( new_adjustment );
}

void ChatWindow::OnCloseButtonClick() {
	OnCloseClick();
}
