#include "OptionsDocumentController.hpp"

#include <SFML/System/Clock.hpp>
#include <Rocket/Controls.h>
#include <Rocket/Core.h>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream> // XXX 

OptionsDocumentController::OptionsDocumentController( Rocket::Core::Element& root ) :
	m_root( root ),
	m_username_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "username" ) ) ),
	m_serial_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "serial" ) ) ),
	m_invert_mouse_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "invert_mouse" ) ) ),
	m_sensitivity_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "sensitivity" ) ) ),
	m_sensitivity_number_element( dynamic_cast<Rocket::Core::Element*>( root.GetElementById( "sensitivity_number" ) ) ),
	m_bindings_data_grid( dynamic_cast<Rocket::Controls::ElementDataGrid*>( root.GetElementById( "bindings" ) ) )
{
	assert( m_username_element );
	assert( m_serial_element );
	assert( m_invert_mouse_element );
	assert( m_sensitivity_element );
	assert( m_sensitivity_number_element );
	assert( m_bindings_data_grid );

	// Events.
	m_sensitivity_element->AddEventListener( "change", this );
	m_bindings_data_grid->AddEventListener( "click", this );
}

OptionsDocumentController::~OptionsDocumentController() {
	// Detach event listener.
	m_sensitivity_element->RemoveEventListener( "change", this );
}

void OptionsDocumentController::serialize( const UserSettings& user_settings ) {
	// Setup data source.
	m_bindings_data_source.reset( new BindingDataSource( user_settings.get_controls() ) );

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
	m_bindings_data_grid->SetDataSource( "bindings.we" );
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
	else if( element->GetId() == "bindings" ) {
		std::cout << "Clicked" << std::endl;
	}
}
