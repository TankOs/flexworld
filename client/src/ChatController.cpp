#include "ChatController.hpp"
#include "RocketUtils.hpp"

#include <SFML/System/Utf.hpp>
#include <SFML/System/String.hpp>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <cassert>

ChatController::ChatController( Rocket::Core::Element* root ) :
	m_root( root ),
	m_send_element( root->GetElementById( "send" ) ),
	m_message_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root->GetElementById( "message" ) ) ),
	m_tabset_element( dynamic_cast<Rocket::Controls::ElementTabSet*>( root->GetElementById( "tabset" ) ) )
{
	assert( m_root );
	assert( m_send_element );
	assert( m_message_element );
	assert( m_tabset_element );

	m_root->AddReference();

	// Event listener.
	m_send_element->AddEventListener( "click", this );
}

ChatController::~ChatController() {
	m_send_element->RemoveEventListener( "click", this );
	m_root->RemoveReference();
}

sf::String ChatController::get_message() const {
	// Convert chat message to SFML string.
	Rocket::Core::String source_string = m_message_element->GetValue();
	std::basic_string<sf::Uint32> utf32_string;

	sf::Utf8::toUtf32(
		source_string.CString(),
		source_string.CString() + source_string.Length(),
		std::back_inserter( utf32_string )
	);

	return sf::String( utf32_string );
}

void ChatController::set_message( const sf::String& message ) {
	// Convert to UTF-8.
	std::string u8_message;

	sf::Utf32::toUtf8(
		message.begin(),
		message.end(),
		std::back_inserter( u8_message )
	);

	m_message_element->SetValue( u8_message.c_str() );
}

void ChatController::focus_message_box() {
	m_message_element->Focus();
}

void ChatController::ProcessEvent( Rocket::Core::Event& event ) {
	Rocket::Core::Element* element = event.GetTargetElement();

	if( element->GetId() == "send" ) {
		if( on_send_click ) {
			on_send_click();
		}
	}
}

void ChatController::add_message( const sf::String& message, const std::string& channel ) {
	// Check if channel exists.
	ChannelArray::iterator chan_iter = std::find( m_channels.begin(), m_channels.end(), channel );

	if( chan_iter == m_channels.end() ) {
		// Create new channel.
		ChannelInfo info;
		info.name = channel;

		std::size_t next_tab_idx = m_channels.size();

		// Create tab.
		Rocket::Core::Element* tab_element = m_tabset_element->GetOwnerDocument()->CreateElement( "tab" );

		tab_element->SetInnerRML( channel.c_str() );
		m_tabset_element->SetTab( static_cast<int>( next_tab_idx ), tab_element );
		tab_element->RemoveReference();

		// Create panel.
		info.panel = m_tabset_element->GetOwnerDocument()->CreateElement( "panel" );

		m_tabset_element->SetPanel( static_cast<int>( next_tab_idx ), info.panel );
		info.panel->RemoveReference();

		m_channels.push_back( info );
		chan_iter = --m_channels.end();
	}

	assert( chan_iter != m_channels.end() );
	ChannelInfo& info = *chan_iter;

	// Convert message to UTF-8.
	sf::String escaped_message = escape_rml_string( message );
	std::string u8_message;

	sf::Utf32::toUtf8( escaped_message.begin(), escaped_message.end(), std::back_inserter( u8_message ) );

	// Create line element.
	Rocket::Core::Element* line_element = m_tabset_element->GetOwnerDocument()->CreateElement( "line" );

	line_element->SetInnerRML( u8_message.c_str() );
	info.panel->AppendChild( line_element );
	line_element->RemoveReference();

	// Prune lines.
	prune_lines( info.panel, 30 );

	// Autoscroll.
	info.panel->SetScrollTop( info.panel->GetScrollHeight() );
}

bool ChatController::ChannelInfo::operator==( const std::string& other_name ) const {
	return name == other_name;
}

bool ChatController::has_active_channel() const {
	return m_tabset_element->GetNumTabs() > 0;
}

const std::string& ChatController::get_active_channel() const {
	assert( has_active_channel() );

	return m_channels[m_tabset_element->GetActiveTab()].name;
}

void ChatController::prune_lines( Rocket::Core::Element* parent, std::size_t num_max_lines ) {
	while( static_cast<std::size_t>( parent->GetNumChildren() ) > num_max_lines ) {
		parent->RemoveChild( parent->GetFirstChild() );
	}
}
