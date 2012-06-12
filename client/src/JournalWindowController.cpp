#include "JournalWindowController.hpp"

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <SFML/System/Utf.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <algorithm>
#include <cassert>

JournalWindowController::JournalWindowController( Rocket::Core::Element* root ) :
	m_root( root )
{
	m_root->AddReference();
}

JournalWindowController::~JournalWindowController() {
	m_root->RemoveReference();
}

void JournalWindowController::add_message( const sf::String& message, const std::string& channel ) {
	Rocket::Controls::ElementTabSet* tabset_element = dynamic_cast<Rocket::Controls::ElementTabSet*>( m_root->GetElementById( "tabset" ) );
	assert( tabset_element );

	// Check if channel exists.
	ChannelArray::iterator chan_iter = std::find( m_channels.begin(), m_channels.end(), channel );

	if( chan_iter == m_channels.end() ) {
		// Create new channel.
		ChannelInfo info;
		info.name = channel;

		std::size_t next_tab_idx = m_channels.size();

		// Create tab.
		Rocket::Core::Element* tab_element = tabset_element->GetOwnerDocument()->CreateElement( "tab" );

		tab_element->SetInnerRML( channel.c_str() );
		tabset_element->SetTab( static_cast<int>( next_tab_idx ), tab_element );
		tab_element->RemoveReference();

		// Create panel.
		info.panel = tabset_element->GetOwnerDocument()->CreateElement( "panel" );

		tabset_element->SetPanel( static_cast<int>( next_tab_idx ), info.panel );
		info.panel->RemoveReference();

		m_channels.push_back( info );
		chan_iter = --m_channels.end();
	}

	assert( chan_iter != m_channels.end() );
	ChannelInfo& info = *chan_iter;

	// Convert message to UTF-8.
	std::string u8_message;

	sf::Utf32::toUtf8( message.begin(), message.end(), std::back_inserter( u8_message ) );

	// Create line element.
	Rocket::Core::Element* line_element = tabset_element->GetOwnerDocument()->CreateElement( "line" );

	line_element->SetInnerRML( u8_message.c_str() );
	info.panel->AppendChild( line_element );
	line_element->RemoveReference();
}

bool JournalWindowController::ChannelInfo::operator==( const std::string& other_name ) const {
	return name == other_name;
}

