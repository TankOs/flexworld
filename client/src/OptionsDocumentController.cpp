#include "OptionsDocumentController.hpp"

#include <SFML/System/Clock.hpp>
#include <Rocket/Controls.h>
#include <Rocket/Core.h>
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
	m_bindings_element( root.GetElementById( "bindings" ) )
{
	assert( m_username_element );
	assert( m_serial_element );
	assert( m_invert_mouse_element );
	assert( m_sensitivity_element );
	assert( m_sensitivity_number_element );

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
}

OptionsDocumentController::~OptionsDocumentController() {
	// Detach event listener.
	m_sensitivity_element->RemoveEventListener( "change", this );
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
}

void OptionsDocumentController::create_binding_elements( const std::string& description, Controls::Action action ) {
	using Rocket::Core::Element;

	assert( m_bindings_element );

	if( !description.empty() && action != Controls::UNMAPPED ) {
		Element* action_element = m_bindings_element->GetOwnerDocument()->CreateElement( "action" );
		action_element->SetInnerRML( description.c_str() );

		Element* button_element = m_bindings_element->GetOwnerDocument()->CreateElement( "button" );
		button_element->SetInnerRML( "Not assigned" );

		m_bindings_element->AppendChild( action_element );
		m_bindings_element->AppendChild( button_element );

		action_element->RemoveReference();
		button_element->RemoveReference();
	}

	Element* br_element = m_bindings_element->GetOwnerDocument()->CreateElement( "br" );

	m_bindings_element->AppendChild( br_element );
	br_element->RemoveReference();
}
