#include <GL/glew.h>

#include "OptionsDocumentController.hpp"
#include "KeyNames.hpp"

#include <Rocket/Controls.h>
#include <Rocket/Core.h>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <iostream> // XXX 

OptionsDocumentController::OptionsDocumentController( Rocket::Core::Element& root ) :
	m_root( root ),
	m_username_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "username" ) ) ),
	m_serial_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "serial" ) ) ),
	m_invert_mouse_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "invert_mouse" ) ) ),
	m_sensitivity_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "sensitivity" ) ) ),
	m_sensitivity_number_element( dynamic_cast<Rocket::Core::Element*>( root.GetElementById( "sensitivity_number" ) ) ),
	m_bindings_element( root.GetElementById( "bindings" ) ),
	m_vsync_element( root.GetElementById( "vsync" ) ),
	m_fps_div_element( root.GetElementById( "fps_div" ) ),
	m_fps_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "fps" ) ) ),
	m_fps_number_element( root.GetElementById( "fps_number" ) ),
	m_fov_element( dynamic_cast<Rocket::Controls::ElementFormControlInput*>( root.GetElementById( "fov" ) ) ),
	m_fov_number_element( root.GetElementById( "fov_number" ) ),
	m_fullscreen_element( root.GetElementById( "fullscreen" ) ),
	m_resolution_element( dynamic_cast<Rocket::Controls::ElementFormControlSelect*>( root.GetElementById( "resolution" ) ) ),
	m_texture_filter_element( dynamic_cast<Rocket::Controls::ElementFormControlSelect*>( root.GetElementById( "texture_filter" ) ) ),
	m_anisotropic_filter_element( dynamic_cast<Rocket::Controls::ElementFormControlSelect*>( root.GetElementById( "af" ) ) ),
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
	assert( m_vsync_element );
	assert( m_fps_div_element );
	assert( m_fps_element );
	assert( m_fps_number_element );
	assert( m_fov_element );
	assert( m_fov_number_element );
	assert( m_fullscreen_element );
	assert( m_resolution_element );
	assert( m_texture_filter_element );
	assert( m_anisotropic_filter_element );
	assert( m_close_element );
	assert( m_save_element );

	m_root.AddReference();

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
	create_binding_elements( "Toggle desk", Controls::DESK );
	create_binding_elements( "Chat", Controls::CHAT );

	// Events.
	m_sensitivity_element->AddEventListener( "change", this );
	m_vsync_element->AddEventListener( "change", this );
	m_fps_element->AddEventListener( "change", this );
	m_fov_element->AddEventListener( "change", this );
	m_close_element->AddEventListener( "click", this );
	m_save_element->AddEventListener( "click", this );

	// Enumerate resolutions.
	const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();
	const sf::VideoMode& desktop_mode = sf::VideoMode::getDesktopMode();

	for( std::size_t idx = 0; idx < modes.size(); ++idx ) {
		const sf::VideoMode& mode = modes[idx];

		// Skip too low resolutions and where BPP does not match.
		if(
			mode.bitsPerPixel != desktop_mode.bitsPerPixel ||
			mode.width < 800 ||
			mode.height < 600
		) {
			continue;
		}

		// Remember video mode.
		m_video_modes.push_back( mode );

		// Add <select> option.
		std::stringstream sstr;
		sstr << mode.width << " x " << mode.height;

		m_resolution_element->Add( sstr.str().c_str(), std::to_string( m_video_modes.size() - 1 ).c_str() );
	}

	// Enumerate anisotropic filter levels.
	float max_af_level = 0.0f;

	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_af_level );

	for( float level = 1.0f; level <= max_af_level; level *= 2.0f ) {
		std::stringstream sstr;
		sstr << static_cast<int>( level ) << "x";

		m_anisotropic_filter_element->Add( sstr.str().c_str(), std::to_string( static_cast<int>( level ) ).c_str() );
	}
}

OptionsDocumentController::~OptionsDocumentController() {
	// Remove binding button event listener.
	Rocket::Core::ElementList buttons;

	m_bindings_element->GetElementsByTagName( buttons, "button" );

	Rocket::Core::ElementList::iterator button_iter( buttons.begin() );
	Rocket::Core::ElementList::iterator button_iter_end( buttons.end() );
	
	for( ; button_iter != button_iter_end; ++button_iter ) {
		(*button_iter)->RemoveEventListener( "click", this );
	}


	m_sensitivity_element->RemoveEventListener( "change", this );
	m_vsync_element->RemoveEventListener( "change", this );
	m_fps_element->RemoveEventListener( "change", this );
	m_fov_element->RemoveEventListener( "change", this );
	m_close_element->RemoveEventListener( "click", this );
	m_save_element->RemoveEventListener( "click", this );

	m_root.RemoveReference();
}

void OptionsDocumentController::serialize( const UserSettings& user_settings ) {
	// Store local copy.
	m_user_settings = user_settings;

	// Account.
	m_username_element->SetValue( m_user_settings.get_username().c_str() );
	m_serial_element->SetValue( m_user_settings.get_serial().c_str() );

	// Controls.
	if( m_user_settings.get_controls().is_mouse_inverted() ) {
		m_invert_mouse_element->SetAttribute( "checked", "checked" );
	}
	else {
		m_invert_mouse_element->RemoveAttribute( "checked" );
	}

	update_range_numbers();
	m_sensitivity_element->SetValue( std::to_string( std::floor( m_user_settings.get_controls().get_mouse_sensitivity() * 10.0f ) ).c_str() );

	update_binding_labels();

	// Video.

	if( m_user_settings.is_vsync_enabled() ) {
		m_vsync_element->SetAttribute( "checked", "checked" );
		m_fps_div_element->SetProperty( "display", "none" );
	}
	else {
		m_vsync_element->RemoveAttribute( "display" );
		m_fps_div_element->RemoveProperty( "display" );
	}

	m_fps_element->SetValue( std::to_string( m_user_settings.get_fps_limit() ).c_str() );
	m_fov_element->SetValue( std::to_string( std::floor( m_user_settings.get_fov() ) ).c_str() );

	// Pick video mode.
	int new_mode_idx = 0;

	for( std::size_t mode_idx = 0; mode_idx < m_video_modes.size(); ++mode_idx ) {
		const sf::VideoMode& mode = m_video_modes[mode_idx];

		if(
			mode.width == m_user_settings.get_video_mode().width &&
			mode.height == m_user_settings.get_video_mode().height
		) {
			new_mode_idx = static_cast<int>( mode_idx );
			break;
		}
	}

	// Fullscreen.
	if( m_user_settings.is_fullscreen_enabled() ) {
		m_fullscreen_element->SetAttribute( "checked", "checked" );
	}
	else {
		m_fullscreen_element->RemoveAttribute( "checked" );
	}

	m_resolution_element->SetSelection( new_mode_idx );

	// Texture & anisotropic filter.
	m_texture_filter_element->SetSelection( static_cast<int>( m_user_settings.get_texture_filter() ) );
	m_anisotropic_filter_element->SetSelection( static_cast<int>( m_user_settings.get_anisotropy_level() ) );
}

void OptionsDocumentController::update_range_numbers() {
	{
		std::stringstream sstr;
		sstr << static_cast<int>( std::floor( m_user_settings.get_controls().get_mouse_sensitivity() * 10.0f ) );
		m_sensitivity_number_element->SetInnerRML( sstr.str().c_str() );
	}
	{
		std::stringstream sstr;
		sstr << static_cast<int>( m_user_settings.get_fps_limit() );
		m_fps_number_element->SetInnerRML( sstr.str().c_str() );
	}
	{
		std::stringstream sstr;
		sstr << static_cast<int>( m_user_settings.get_fov() );
		m_fov_number_element->SetInnerRML( sstr.str().c_str() );
	}
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
		m_user_settings.get_controls().set_mouse_sensitivity( std::floor( number ) / 10.0f );

		if( update_timer.getElapsedTime() >= sf::milliseconds( 20 ) ) {
			update_range_numbers();
			update_timer.restart();
		}
	}
	else if( element->GetId() == "vsync" ) {
		if( element->HasAttribute( "checked" ) ) {
			m_fps_div_element->SetProperty( "display", "none" );
		}
		else {
			m_fps_div_element->RemoveProperty( "display" );
		}
	}
	else if( element->GetId() == "fps" ) {
		std::stringstream sstr( m_fps_element->GetValue().CString() );
		float number = 0;

		sstr >> number;
		m_user_settings.set_fps_limit( static_cast<unsigned int>( std::floor( number ) ) );

		if( update_timer.getElapsedTime() >= sf::milliseconds( 20 ) ) {
			update_range_numbers();
			update_timer.restart();
		}
	}
	else if( element->GetId() == "fov" ) {
		std::stringstream sstr( m_fov_element->GetValue().CString() );
		float number = 0;

		sstr >> number;
		m_user_settings.set_fov( static_cast<uint8_t>( std::floor( number ) ) );

		if( update_timer.getElapsedTime() >= sf::milliseconds( 20 ) ) {
			update_range_numbers();
			update_timer.restart();
		}
	}
	else if( element->GetId() == "close" ) {
		if( on_reject ) {
			on_reject();
		}
	}
	else if( element->GetId() == "save" ) {
		// Deserialize.
		m_user_settings.set_username( std::string( m_username_element->GetValue().CString() ) );
		m_user_settings.set_serial( std::string( m_serial_element->GetValue().CString() ) );

		m_user_settings.get_controls().set_mouse_inverted( m_invert_mouse_element->HasAttribute( "checked" ) );

		m_user_settings.enable_vsync( m_vsync_element->HasAttribute( "checked" ) );
		m_user_settings.enable_fullscreen( m_fullscreen_element->HasAttribute( "checked" ) );
		m_user_settings.set_video_mode( m_video_modes[m_resolution_element->GetSelection()] );
		m_user_settings.set_texture_filter( static_cast<TextureFilter>( m_texture_filter_element->GetSelection() ) );
		m_user_settings.set_anisotropy_level( static_cast<uint8_t>( m_anisotropic_filter_element->GetSelection() ) );

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
