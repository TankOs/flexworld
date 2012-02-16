#include <cstdint>

#include "OptionsWindow.hpp"
#include "KeyNames.hpp"

OptionsWindow::OptionsWindow( const UserSettings& user_settings ) :
	Window( Window::Toplevel ),
	m_user_settings( user_settings ),
	m_event_processed( false ),
	m_next_action( Controls::UNMAPPED )
{
}

OptionsWindow::Ptr OptionsWindow::Create( const UserSettings& user_settings ) {
	Ptr window( new OptionsWindow( user_settings ) );

	// Widgets.
	window->SetTitle( L"Options" );

	sfg::Button::Ptr ok_button( sfg::Button::Create( L"Apply changes" ) );
	sfg::Button::Ptr cancel_button( sfg::Button::Create( L"Cancel" ) );

	// Account.
	window->m_username_entry = sfg::Entry::Create( window->m_user_settings.get_username() );
	window->m_serial_entry = sfg::Entry::Create( window->m_user_settings.get_serial() );

	// Controls.
	window->m_mouse_inverted_check = sfg::CheckButton::Create( L"Inverted" );
	window->m_mouse_inverted_check->SetActive( window->m_user_settings.get_controls().is_mouse_inverted() );
	window->m_mouse_sensitivity_scale = sfg::Scale::Create( .01f, 2.f, .01f );
	window->m_mouse_sensitivity_scale->SetValue( window->m_user_settings.get_controls().get_mouse_sensitivity() );
	window->m_mouse_sensitivity_label = sfg::Label::Create( L"--" );

	window->m_mouse_sensitivity_label->SetRequisition( sf::Vector2f( 25.f, 0 ) );

	// Setup action buttons.
	window->m_action_buttons[Controls::WALK_FORWARD] = sfg::Button::Create();
	window->m_action_buttons[Controls::WALK_BACKWARD] = sfg::Button::Create();
	window->m_action_buttons[Controls::STRAFE_LEFT] = sfg::Button::Create();
	window->m_action_buttons[Controls::STRAFE_RIGHT] = sfg::Button::Create();
	window->m_action_buttons[Controls::JUMP] = sfg::Button::Create();
	window->m_action_buttons[Controls::CROUCH] = sfg::Button::Create();
	window->m_action_buttons[Controls::USE] = sfg::Button::Create();
	window->m_action_buttons[Controls::DROP] = sfg::Button::Create();
	window->m_action_buttons[Controls::PRIMARY_ATTACK] = sfg::Button::Create();
	window->m_action_buttons[Controls::SECONDARY_ATTACK] = sfg::Button::Create();
	window->m_action_buttons[Controls::INVENTORY] = sfg::Button::Create();
	window->m_action_buttons[Controls::CHAT] = sfg::Button::Create();

	ActionButtonMap::iterator ab_iter( window->m_action_buttons.begin() );
	ActionButtonMap::iterator ab_iter_end( window->m_action_buttons.end() );

	// Connect signals, set active mapping labels and store Button -> Action
	// relations.
	for( ; ab_iter != ab_iter_end; ++ab_iter ) {
		ab_iter->second->OnClick.Connect( &OptionsWindow::on_action_button_click, &*window );
		window->m_button_actions[ab_iter->second] = ab_iter->first;
	}

	window->m_waiting_for_input_label = sfg::Label::Create( L"Press a key or mouse button (hit ESC to cancel)." );
	window->m_waiting_for_input_label->Show( false );

	// Video.
	window->m_enable_vsync_check = sfg::CheckButton::Create( L"Vertical sync (avoids tearing)" );
	window->m_fps_limit_scale = sfg::Scale::Create( 1.0f, 200.0f, 1.0f, sfg::Scale::HORIZONTAL );
	window->m_fps_limit_label = sfg::Label::Create( L"(--)" );
	window->m_fov_scale = sfg::Scale::Create( UserSettings::MIN_FOV, UserSettings::MAX_FOV, 1.0f, sfg::Scale::HORIZONTAL );
	window->m_fov_label = sfg::Label::Create( L"(--)" );

	// Layout //////////////////////////////////////////////////

	// Account.
	sfg::Table::Ptr acc_table( sfg::Table::Create() );
	acc_table->SetRowSpacings( 10.f );
	acc_table->SetColumnSpacings( 10.f );
	acc_table->Attach( sfg::Label::Create( L"Username:" ), sf::Rect<sf::Uint32>( 0, 0, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	acc_table->Attach( window->m_username_entry, sf::Rect<sf::Uint32>( 1, 0, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	acc_table->Attach( sfg::Label::Create( L"Serial:" ), sf::Rect<sf::Uint32>( 0, 1, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	acc_table->Attach( window->m_serial_entry, sf::Rect<sf::Uint32>( 1, 1, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	sfg::Frame::Ptr account_frame( sfg::Frame::Create( L"Account data" ) );
	account_frame->Add( acc_table );

	sfg::Box::Ptr account_page_box = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );
	account_page_box->Pack( account_frame, false );

	// Controls.
	sfg::Table::Ptr mouse_table( sfg::Table::Create() );
	mouse_table->SetRowSpacings( 10.f );
	mouse_table->SetColumnSpacings( 10.f );

	mouse_table->Attach( sfg::Label::Create( L"Sensitivity:" ), sf::Rect<sf::Uint32>( 0, 0, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	mouse_table->Attach( window->m_mouse_sensitivity_scale, sf::Rect<sf::Uint32>( 1, 0, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	mouse_table->Attach( window->m_mouse_sensitivity_label, sf::Rect<sf::Uint32>( 2, 0, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	mouse_table->Attach( window->m_mouse_inverted_check, sf::Rect<sf::Uint32>( 1, 1, 2, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	sfg::Frame::Ptr mouse_frame( sfg::Frame::Create( L"Mouse" ) );
	mouse_frame->Add( mouse_table );

	sfg::Table::Ptr bindings_table( sfg::Table::Create() );
	bindings_table->SetRowSpacings( 10.f );
	bindings_table->SetColumnSpacings( 10.f );

	uint32_t row_index( 0 );
	bindings_table->Attach( sfg::Label::Create( L"Forward:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Backward:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Strafe left:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Strafe right:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Jump:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Crouch:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	bindings_table->Attach( sfg::Label::Create( L"Use/Take:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Drop:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Primary attack:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Secondary attack:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Inventory:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	bindings_table->Attach( sfg::Label::Create( L"Chat:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	bindings_table->Attach( window->m_action_buttons[Controls::WALK_FORWARD], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::WALK_BACKWARD], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::STRAFE_LEFT], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::STRAFE_RIGHT], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::JUMP], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::CROUCH], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	row_index = 0;
	bindings_table->Attach( window->m_action_buttons[Controls::USE], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::DROP], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::PRIMARY_ATTACK], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::SECONDARY_ATTACK], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::INVENTORY], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	bindings_table->Attach( window->m_action_buttons[Controls::CHAT], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	sfg::Frame::Ptr bindings_frame( sfg::Frame::Create( L"Bindings" ) );
	bindings_frame->Add( bindings_table );

	sfg::Box::Ptr controls_page_box = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );
	controls_page_box->Pack( mouse_frame, false );
	controls_page_box->Pack( bindings_frame, false );
	controls_page_box->Pack( window->m_waiting_for_input_label, false );

	// Video.
	sfg::Box::Ptr fps_limit_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f ) );
	fps_limit_box->Pack( window->m_fps_limit_scale, true );
	fps_limit_box->Pack( window->m_fps_limit_label, false );

	sfg::Box::Ptr fov_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f ) );
	fov_box->Pack( window->m_fov_scale, true );
	fov_box->Pack( window->m_fov_label, false );

	sfg::Table::Ptr general_table( sfg::Table::Create() );
	general_table->SetRowSpacings( 10.0f );
	general_table->SetColumnSpacings( 10.0f );

	general_table->Attach( window->m_enable_vsync_check, sf::Rect<sf::Uint32>( 1, 0, 1, 1 ), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL );

	general_table->Attach( sfg::Label::Create( L"FPS limit:" ), sf::Rect<sf::Uint32>( 0, 1, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	general_table->Attach( fps_limit_box, sf::Rect<sf::Uint32>( 1, 1, 1, 1 ), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL );

	general_table->Attach( sfg::Label::Create( L"Field Of View:" ), sf::Rect<sf::Uint32>( 0, 2, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	general_table->Attach( fov_box, sf::Rect<sf::Uint32>( 1, 2, 1, 1 ), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL );

	sfg::Frame::Ptr general_frame( sfg::Frame::Create( L"General" ) );
	general_frame->Add( general_table );

	sfg::Box::Ptr video_page_box( sfg::Box::Create( sfg::Box::VERTICAL, 5.0f ) );
	video_page_box->Pack( general_frame, false );

	// Notebook.
	sfg::Notebook::Ptr notebook( sfg::Notebook::Create() );
	notebook->AppendPage( account_page_box, sfg::Label::Create( L"Account" ) );
	notebook->AppendPage( controls_page_box, sfg::Label::Create( L"Controls" ) );
	notebook->AppendPage( video_page_box, sfg::Label::Create( L"Video" ) );

	// ---
	sfg::Box::Ptr bottom_button_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.f ) );
	bottom_button_box->Pack( cancel_button, false );
	bottom_button_box->Pack( ok_button, false );

	sfg::Alignment::Ptr alignment( sfg::Alignment::Create() );
	alignment->Add( bottom_button_box );
	alignment->SetAlignment( sf::Vector2f( 1.0f, 0.0f ) );
	alignment->SetScale( sf::Vector2f( 0.0f, 0.0f ) );

	sfg::Box::Ptr content_box( sfg::Box::Create( sfg::Box::VERTICAL, 10.f ) );
	content_box->Pack( notebook, true );
	content_box->Pack( alignment, false );

	window->Add( content_box );

	// Signals.
	window->m_mouse_sensitivity_scale->GetAdjustment()->OnChange.Connect( &OptionsWindow::on_sensitivity_change, &*window );

	ok_button->OnClick.Connect( &OptionsWindow::on_ok_click, &*window );
	cancel_button->OnClick.Connect( &OptionsWindow::on_cancel_click, &*window );

	window->m_fps_limit_scale->GetAdjustment()->OnChange.Connect( &OptionsWindow::on_fps_limit_change, &*window );
	window->m_fov_scale->GetAdjustment()->OnChange.Connect( &OptionsWindow::on_fov_change, &*window );

	// Init.
	window->m_enable_vsync_check->SetActive( window->m_user_settings.is_vsync_enabled() );
	window->m_fps_limit_scale->SetValue( static_cast<float>( window->m_user_settings.get_fps_limit() ) );
	window->m_fps_limit_label->SetRequisition( sf::Vector2f( 50.0f, 0.0f ) );
	window->m_fov_scale->SetValue( static_cast<float>( window->m_user_settings.get_fov() ) );
	window->m_fov_label->SetRequisition( sf::Vector2f( 50.0f, 0.0f ) );

	window->refresh_action_button_labels();
	window->on_sensitivity_change();

	window->on_fps_limit_change();
	window->on_fov_change();

	// Cycle through all notebook pages so that the size is maximized.
	while( notebook->GetCurrentPage() + 1 < notebook->GetPageCount() ) {
		notebook->NextPage();
	}

	notebook->SetCurrentPage( 0 );

	return window;
}

void OptionsWindow::on_ok_click() {
	// Update user settings.
	m_user_settings.set_username( m_username_entry->GetText() );
	m_user_settings.set_serial( m_serial_entry->GetText() );
	m_user_settings.enable_vsync( m_enable_vsync_check->IsActive() );
	m_user_settings.set_fps_limit( static_cast<uint32_t>( m_fps_limit_scale->GetValue() ) );
	m_user_settings.set_fov( static_cast<uint8_t>( m_fov_scale->GetValue() ) );
	m_user_settings.get_controls().set_mouse_inverted( m_mouse_inverted_check->IsActive() );
	m_user_settings.get_controls().set_mouse_sensitivity( m_mouse_sensitivity_scale->GetValue() );

	OnAccept();
}

void OptionsWindow::on_cancel_click() {
	OnReject();
}

bool OptionsWindow::is_event_processed() const {
	return m_event_processed;
}

void OptionsWindow::HandleEvent( const sf::Event& event ) {
	m_event_processed = false;

	if( m_next_action != Controls::UNMAPPED && (event.Type == sf::Event::MouseButtonPressed || event.Type == sf::Event::KeyPressed) ) {
		// Mark every key press and mouse button event as processed if waiting for an
		// action mapping.
		m_event_processed = true;

		// Map key/button.
		if( event.Type == sf::Event::KeyPressed ) {
			// Do not process ESC, as it cancels the binding.
			if( event.Key.Code != sf::Keyboard::Escape ) {
				m_user_settings.get_controls().map_key( event.Key.Code, m_next_action );
			}
		}
		else {
			m_user_settings.get_controls().map_button( event.MouseButton.Button, m_next_action );
		}

		// Update labels (need to update all because another mapping might got
		// removed).
		refresh_action_button_labels();

		m_waiting_for_input_label->Show( false );
		m_next_action = Controls::UNMAPPED;
	}
	else {
		Window::HandleEvent( event );
	}
}

void OptionsWindow::on_action_button_click() {
	m_active_action_button = sfg::DynamicPointerCast<sfg::Button>( sfg::Context::Get().GetActiveWidget() );
	m_next_action = m_button_actions[m_active_action_button];

	m_active_action_button->SetLabel( "..." );
	m_waiting_for_input_label->Show( true );
}

void OptionsWindow::refresh_action_button_labels() {
	// Reset all buttons first (so old bindings won't survive).
	ButtonActionMap::iterator ba_iter( m_button_actions.begin() );
	ButtonActionMap::iterator ba_iter_end( m_button_actions.end() );
	
	for( ; ba_iter != ba_iter_end; ++ba_iter ) {
		ba_iter->first->SetLabel( "Not bound" );
	}

	// Process key bindings.
	Controls::ControlMap::const_iterator key_iter( m_user_settings.get_controls().keys_begin() );
	Controls::ControlMap::const_iterator key_iter_end( m_user_settings.get_controls().keys_end() );

	for( ; key_iter != key_iter_end; ++key_iter ) {
		std::string name( get_key_name( key_iter->first ) );
		if( name == "" ) {
			name = "???";
		}

		m_action_buttons[key_iter->second]->SetLabel( name );
	}

	// Process button bindings.
	Controls::ControlMap::const_iterator button_iter( m_user_settings.get_controls().buttons_begin() );
	Controls::ControlMap::const_iterator button_iter_end( m_user_settings.get_controls().buttons_end() );

	for( ; button_iter != button_iter_end; ++button_iter ) {
		std::string name( get_button_name( button_iter->first ) );
		if( name == "" ) {
			name = "???";
		}

		m_action_buttons[button_iter->second]->SetLabel( name );
	}
}

const UserSettings& OptionsWindow::get_user_settings() const {
	return m_user_settings;
}

void OptionsWindow::on_sensitivity_change() {
	std::stringstream sstr;
	sstr << std::fixed << std::setprecision( 2 ) << m_mouse_sensitivity_scale->GetValue();
	m_mouse_sensitivity_label->SetText( sstr.str() );
}

void OptionsWindow::on_fps_limit_change() {
	std::stringstream sstr;

	sstr << static_cast<int>( m_fps_limit_scale->GetValue() );
	m_fps_limit_label->SetText( sstr.str() );
}

void OptionsWindow::on_fov_change() {
	std::stringstream sstr;

	sstr << static_cast<int>( m_fov_scale->GetValue() ) << " deg";
	m_fov_label->SetText( sstr.str() );
}
