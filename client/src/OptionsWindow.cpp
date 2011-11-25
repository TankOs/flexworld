#include "OptionsWindow.hpp"
#include "KeyNames.hpp"

OptionsWindow::OptionsWindow() :
	Window(),
	m_event_processed( false ),
	m_next_action( Controls::UNMAPPED )
{
}

OptionsWindow::Ptr OptionsWindow::Create() {
	Ptr window( new OptionsWindow );

	// Widgets.
	window->SetBorderWidth( 10.f );
	window->SetTitle( L"Options" );

	sfg::Button::Ptr account_button( sfg::Button::Create( L"Account" ) );
	sfg::Button::Ptr controls_button( sfg::Button::Create( L"Controls" ) );
	sfg::Button::Ptr graphics_button( sfg::Button::Create( L"Video" ) );
	sfg::Button::Ptr audio_button( sfg::Button::Create( L"Audio" ) );

	sfg::Button::Ptr ok_button( sfg::Button::Create( L"OK" ) );
	sfg::Button::Ptr cancel_button( sfg::Button::Create( L"Cancel" ) );

	// Account.
	window->m_username_entry = sfg::Entry::Create();
	window->m_serial_entry = sfg::Entry::Create();

	// Controls.
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

	// Layout.
	sfg::Box::Ptr top_button_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.f ) );
	top_button_box->Pack( account_button, true );
	top_button_box->Pack( controls_button, true );
	top_button_box->Pack( graphics_button, true );
	top_button_box->Pack( audio_button, true );

	// Account.
	sfg::Table::Ptr acc_table( sfg::Table::Create() );
	acc_table->SetRowSpacings( 10.f );
	acc_table->SetColumnSpacings( 10.f );
	acc_table->Attach( sfg::Label::Create( L"Username:" ), sf::Rect<sf::Uint32>( 0, 0, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	acc_table->Attach( window->m_username_entry, sf::Rect<sf::Uint32>( 1, 0, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	acc_table->Attach( sfg::Label::Create( L"Serial:" ), sf::Rect<sf::Uint32>( 0, 1, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	acc_table->Attach( window->m_serial_entry, sf::Rect<sf::Uint32>( 1, 1, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	window->m_account_page_box = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );
	window->m_account_page_box->Pack( acc_table, false );

	// Controls.
	sfg::Table::Ptr controls_table( sfg::Table::Create() );
	controls_table->SetRowSpacings( 10.f );
	controls_table->SetColumnSpacings( 10.f );

	uint32_t row_index( 0 );
	controls_table->Attach( sfg::Label::Create( L"Forward:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Backward:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Strafe left:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Strafe right:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Jump:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Crouch:" ), sf::Rect<sf::Uint32>( 0, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	controls_table->Attach( sfg::Label::Create( L"Use/Take:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Drop:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Primary attack:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Secondary attack:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Inventory:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	controls_table->Attach( sfg::Label::Create( L"Chat:" ), sf::Rect<sf::Uint32>( 2, row_index++, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	row_index = 0;
	controls_table->Attach( window->m_action_buttons[Controls::WALK_FORWARD], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::WALK_BACKWARD], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::STRAFE_LEFT], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::STRAFE_RIGHT], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::JUMP], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::CROUCH], sf::Rect<sf::Uint32>( 1, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	row_index = 0;
	controls_table->Attach( window->m_action_buttons[Controls::USE], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::DROP], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::PRIMARY_ATTACK], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::SECONDARY_ATTACK], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::INVENTORY], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );
	controls_table->Attach( window->m_action_buttons[Controls::CHAT], sf::Rect<sf::Uint32>( 3, row_index++, 1, 1 ), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL );

	window->m_controls_page_box = sfg::Box::Create( sfg::Box::VERTICAL, 5.f );
	window->m_controls_page_box->Pack( controls_table, false );
	window->m_controls_page_box->Pack( window->m_waiting_for_input_label, false );

	// ---
	sfg::Box::Ptr bottom_button_box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.f ) );
	bottom_button_box->Pack( cancel_button, false );
	bottom_button_box->Pack( ok_button, false );

	sfg::Box::Ptr content_box( sfg::Box::Create( sfg::Box::VERTICAL, 10.f ) );
	content_box->Pack( top_button_box, false );
	content_box->Pack( window->m_account_page_box, true );
	content_box->Pack( window->m_controls_page_box, true );
	content_box->Pack( bottom_button_box, false );

	window->Add( content_box );

	window->show_page( window->m_account_page_box );

	// Signals.
	account_button->OnClick.Connect( &OptionsWindow::on_account_click, &*window );
	controls_button->OnClick.Connect( &OptionsWindow::on_controls_click, &*window );

	ok_button->OnClick.Connect( &OptionsWindow::on_ok_click, &*window );
	cancel_button->OnClick.Connect( &OptionsWindow::on_cancel_click, &*window );

	// Init.
	window->refresh_action_button_labels();

	return window;
}

void OptionsWindow::show_page( sfg::Widget::Ptr page ) {
	m_account_page_box->Show( false );
	m_controls_page_box->Show( false );

	page->Show( true );
}

void OptionsWindow::on_account_click() {
	show_page( m_account_page_box );
}

void OptionsWindow::on_controls_click() {
	show_page( m_controls_page_box );
}

void OptionsWindow::on_ok_click() {
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
			m_controls.map_key( event.Key.Code, m_next_action );
		}
		else {
			m_controls.map_button( event.MouseButton.Button, m_next_action );
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
	m_active_action_button = std::dynamic_pointer_cast<sfg::Button>( sfg::Context::Get().GetActiveWidget() );
	m_next_action = m_button_actions[m_active_action_button];

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
	Controls::ControlMap::const_iterator key_iter( m_controls.keys_begin() );
	Controls::ControlMap::const_iterator key_iter_end( m_controls.keys_end() );

	for( ; key_iter != key_iter_end; ++key_iter ) {
		std::string name( get_key_name( key_iter->first ) );
		if( name == "" ) {
			name = "???";
		}

		m_action_buttons[key_iter->second]->SetLabel( name );
	}

	// Process button bindings.
	Controls::ControlMap::const_iterator button_iter( m_controls.buttons_begin() );
	Controls::ControlMap::const_iterator button_iter_end( m_controls.buttons_end() );

	for( ; button_iter != button_iter_end; ++button_iter ) {
		std::string name( get_button_name( button_iter->first ) );
		if( name == "" ) {
			name = "???";
		}

		m_action_buttons[button_iter->second]->SetLabel( name );
	}
}
