#include "OptionsDocumentController.hpp"
#include "KeyNames.hpp"

#include <Rocket/Controls.h>
#include <Rocket/Core.h>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <iostream> // XXX 

OptionsDocumentController::OptionsDocumentController( Rocket::Core::Element& root ) :
	m_root( root ),
	m_username_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "username" ) ) ),
	m_serial_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "serial" ) ) ),
	m_invert_mouse_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "invert_mouse" ) ) ),
	m_sensitivity_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "sensitivity" ) ) ),
	m_sensitivity_number_element( dynamic_cast<Rocket::Core::Element*>( root.GetElementById( "sensitivity_number" ) ) ),
	m_bindings_element( root.GetElementById( "bindings" ) ),
	m_resolution_element( root.GetElementById( "resolution" ) ),
	m_close_element( root.GetElementById( "close" ) ),
	m_save_element( root.GetElementById( "save" ) ),
	m_current_action_id( Controls::UNMAPPED ),
	m_is_waiting_for_press( false )
{
	assert( m_username_element );
	assert( m_serial_element );
	assert( m_invert_mouse_element );
	assert( m_sensitivity_element );
	assert( m_sensitivity_number_element );
	assert( m_resolution_element );
	assert( m_close_element );
	assert( m_save_element );

	// Create binding labels + buttons.
	create_binding_elements( "Walk forward", Controls::WALK_FORWARD );
	create_binding_elements( "Walk backward", Controls::WALK_BACKWARD );
	create_binding_elements( "Strafe left", Controls::STRAFE_LEFT );
	create_binding_elements( "Strafe right", Controls::STRAFE_RIGHT );
	create_binding_elements( "Run", Controls::RUN );
	create_binding_elements( "Jump", Controls::JUMP );
	create_binding_elements( "Crouch", Controls::CROUCH );
	create_binding_elements();
	create_binding_elements( "Primary action", Controls::PRIMARY_ACTION );
	create_binding_elements( "Secondary action", Controls::SECONDARY_ACTION );
	create_binding_elements( "Use (hold to pick up)", Controls::USE );
	create_binding_elements( "Drop", Controls::DROP );
	create_binding_elements();
	create_binding_elements( "Toggle desk", Controls::INVENTORY );
	create_binding_elements( "Chat", Controls::CHAT );

	// Events.
	m_sensitivity_element->AddEventListener( "change", this );
	m_close_element->AddEventListener( "click", this );
	m_save_element->AddEventListener( "click", this );

	// Enumerate resolutions.
	const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();
	const sf::VideoMode& desktop_mode = sf::VideoMode::getDesktopMode();

	for( std::size_t idx = 0; idx < modes.size(); ++idx ) {
		const sf::VideoMode& mode = modes[idx];

		// Skip too low resolutions and those that are above the desktop
		// resolution. BPP must also match.
		if(
			mode.width > desktop_mode.width ||
			mode.height > desktop_mode.height  ||
			mode.bitsPerPixel != desktop_mode.bitsPerPixel ||
			mode.width < 800 ||
			mode.height < 600
		) {
			continue;
		}

		std::stringstream sstr;
		sstr << mode.width << " x " << mode.height;

		// Add to <select> element.
		Rocket::Core::Element* option_element = m_resolution_element->GetOwnerDocument()->CreateElement( "option" );
		option_element->SetAttribute( "value", "whatever" );
		option_element->SetInnerRML( sstr.str().c_str() );

		m_resolution_element->AppendChild( option_element );
		option_element->RemoveReference();
	}

	// Enumerate anisotropic filter levels.

}

OptionsDocumentController::~OptionsDocumentController() {
	// Detach event listener.
	m_sensitivity_element->RemoveEventListener( "change", this );
	m_close_element->RemoveEventListener( "close", this );
}

void OptionsDocumentController::serialize( const UserSettings& user_settings ) {
	// Store local copy.
	m_user_settings = user_settings;

	// Set values in GUI.
	m_username_element->SetValue( m_user_settings.get_username().c_str() );
	m_serial_element->SetValue( m_user_settings.get_serial().c_str() );

	if( m_user_settings.get_controls().is_mouse_inverted() ) {
		m_invert_mouse_element->SetAttribute( "checked", "checked" );
	}
	else {
		m_invert_mouse_element->RemoveAttribute( "checked" );
	}

	update_sensitivity_number();
	m_sensitivity_element->SetValue( std::to_string( std::ceil( m_user_settings.get_controls().get_mouse_sensitivity() * 10.0f ) ).c_str() );

	update_binding_labels();
}

void OptionsDocumentController::update_sensitivity_number() {
	std::stringstream sstr;
	sstr << static_cast<int>( std::ceil( m_user_settings.get_controls().get_mouse_sensitivity() * 10.0f ) );

	m_sensitivity_number_element->SetInnerRML( sstr.str().c_str() );
}

void OptionsDocumentController::ProcessEvent( Rocket::Core::Event& event ) {
	static sf::Clock update_timer;

	Rocket::Core::Element* element = event.GetTargetElement();

	if( !element ) {
		return;
	}

	if( element->GetId() == "sensitivity" ) {
		std::stringstream sstr( m_sensitivity_element->GetValue().CString() );
		float number = 0;

		sstr >> number;
		m_user_settings.get_controls().set_mouse_sensitivity( number / 10.0f );

		if( update_timer.getElapsedTime() >= sf::milliseconds( 20 ) ) {
			update_sensitivity_number();
			update_timer.restart();
		}
	}
	else if( element->GetId() == "close" ) {
		if( on_reject ) {
			on_reject();
		}
	}
	else if( element->GetId() == "save" ) {
		if( on_accept ) {
			on_accept();
		}
	}
	else {
		// Check for binding button.
		if( element->IsClassSet( "binding_button" ) ) {
			if( !m_is_waiting_for_press ) {
				element->SetInnerRML( "??? (ESC to cancel)" );

				m_is_waiting_for_press = true;

				// Remember action ID.
				m_current_action_id = static_cast<Controls::Action>( element->GetAttribute( "action_id", static_cast<int>( Controls::UNMAPPED ) ) );
				assert( m_current_action_id != Controls::UNMAPPED );
			}
		}
	}
}

void OptionsDocumentController::create_binding_elements( const std::string& description, Controls::Action action ) {
	using Rocket::Core::Element;

	assert( m_bindings_element );

	if( !description.empty() && action != Controls::UNMAPPED ) {
		Element* action_element = m_bindings_element->GetOwnerDocument()->CreateElement( "action" );
		action_element->SetInnerRML( description.c_str() );

		Element* button_element = m_bindings_element->GetOwnerDocument()->CreateElement( "button" );
		button_element->SetInnerRML( "Not assigned" );
		button_element->SetClass( "binding_button", true );
		button_element->AddEventListener( "click", this );

		// Save action ID in element.
		std::stringstream sstr;
		sstr << static_cast<int>( action );
		button_element->SetAttribute( "action_id", sstr.str().c_str() );

		m_bindings_element->AppendChild( action_element );
		m_bindings_element->AppendChild( button_element );

		// Save mapping.
		m_action_elements[action] = button_element;

		action_element->RemoveReference();
		button_element->RemoveReference();
	}

	Element* br_element = m_bindings_element->GetOwnerDocument()->CreateElement( "br" );

	m_bindings_element->AppendChild( br_element );
	br_element->RemoveReference();
}

bool OptionsDocumentController::is_waiting_for_press() const {
	return m_is_waiting_for_press;
}

void OptionsDocumentController::handle_event( const sf::Event& event ) {
	// Skip event if not waiting for one.
	if( m_is_waiting_for_press == false ) {
		return;
	}

	assert( m_current_action_id != Controls::UNMAPPED );

	bool processed = false;

	if( event.type == sf::Event::KeyReleased ) {
		// Only process if key != escape.
		if( event.key.code != sf::Keyboard::Escape ) {
			// Map key to action.
			m_user_settings.get_controls().map_key( event.key.code, m_current_action_id );
		}

		processed = true;
	}
	else if( event.type == sf::Event::MouseButtonReleased ) {
		// Map button to action.
		m_user_settings.get_controls().map_button( event.mouseButton.button, m_current_action_id );

		processed = true;
	}

	// If event got processed then stop asking for further events and update
	// binding buttons.
	if( processed ) {
		m_is_waiting_for_press = false;
		m_current_action_id = Controls::UNMAPPED;
		update_binding_labels();
	}
}

void OptionsDocumentController::update_binding_labels() {
	// Reset all binding buttons.
	{
		ActionElementMap::iterator button_iter( m_action_elements.begin() );
		ActionElementMap::iterator button_iter_end( m_action_elements.end() );
		
		for( ; button_iter != button_iter_end; ++button_iter ) {
			button_iter->second->SetInnerRML( "NOT ASSIGNED" );
		}
	}

	// Iterate over all keys.
	Controls::ControlMap::const_iterator key_iter( m_user_settings.get_controls().keys_begin() );
	Controls::ControlMap::const_iterator key_iter_end( m_user_settings.get_controls().keys_end() );
	
	for( ; key_iter != key_iter_end; ++key_iter ) {
		// Get button element.
		ActionElementMap::iterator button_iter = m_action_elements.find( key_iter->second );

		// If found set key name of bound action.
		if( button_iter != m_action_elements.end() ) {
			std::string key_name = get_key_name( key_iter->first );

			button_iter->second->SetInnerRML( key_name.c_str() );
		}
	}

	// Iterate over all buttons.
	Controls::ControlMap::const_iterator mouse_button_iter( m_user_settings.get_controls().buttons_begin() );
	Controls::ControlMap::const_iterator mouse_button_iter_end( m_user_settings.get_controls().buttons_end() );
	
	for( ; mouse_button_iter != mouse_button_iter_end; ++mouse_button_iter ) {
		// Get button element.
		ActionElementMap::iterator button_iter = m_action_elements.find( mouse_button_iter->second );

		// If found set button name of bound action.
		if( button_iter != m_action_elements.end() ) {
			std::string button_name = get_button_name( mouse_button_iter->first );

			button_iter->second->SetInnerRML( button_name.c_str() );
		}
	}
}

const UserSettings& OptionsDocumentController::get_user_settings() const {
	return m_user_settings;
}
