#include "UserInterface.hpp"
#include "JournalWindowController.hpp"
#include "RocketEventDispatcher.hpp"
#include "Controls.hpp"

#include <FlexWorld/Config.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <Rocket/Core.h>

UserInterface::UserInterface(
	sf::RenderTarget& render_target,
	const Controls& controls
) :
	m_background_shape( sf::Vector2f( static_cast<float>( render_target.getSize().x ), static_cast<float>( render_target.getSize().y ) ) ),
	m_render_target( render_target ),
	m_controls( controls ),
	m_context( 
		Rocket::Core::CreateContext(
			"default",
			Rocket::Core::Vector2i(
				render_target.getSize().x,
				render_target.getSize().y
			)
		)
	),
	m_background_context( 
		Rocket::Core::CreateContext(
			"background",
			Rocket::Core::Vector2i(
				render_target.getSize().x,
				render_target.getSize().y
			)
		)
	),
	m_chat_context( 
		Rocket::Core::CreateContext(
			"chat",
			Rocket::Core::Vector2i(
				render_target.getSize().x,
				render_target.getSize().y
			)
		)
	),
	m_journal_window_document( nullptr ),
	m_toolbar_document( nullptr ),
	m_chat_prompt_document( nullptr ),
	m_desk_visible( false ),
	m_last_event_consumed( false )
{
	// Load fonts.
	Rocket::Core::FontDatabase::LoadFontFace(
		(flex::ROOT_DATA_DIRECTORY + std::string( "local/gui/Economica-Bold.ttf" )).c_str(),
		"MenuFont",
		Rocket::Core::Font::STYLE_NORMAL,
		Rocket::Core::Font::WEIGHT_NORMAL
	);

	// Load documents.
	m_journal_window_document = m_context->LoadDocument(
		(flex::ROOT_DATA_DIRECTORY + std::string( "local/gui/journal.rml" )).c_str()
	);
	m_chat_prompt_document = m_chat_context->LoadDocument(
		(flex::ROOT_DATA_DIRECTORY + std::string( "local/gui/chat_prompt.rml" )).c_str()
	);

	// Background documents.
	m_toolbar_document = m_background_context->LoadDocument(
		(flex::ROOT_DATA_DIRECTORY + std::string( "local/gui/desk_toolbar.rml" )).c_str()
	);

	// Init documents.
	m_toolbar_document->Show();

	// Prepare controller.
	m_journal_controller.reset( new JournalWindowController( m_journal_window_document ) );

	// Background shape.
	m_background_shape.setFillColor( sf::Color( 0, 0, 0, 130 ) );
}

UserInterface::~UserInterface() {
	m_chat_prompt_document->RemoveReference();
	m_toolbar_document->RemoveReference();
	m_journal_window_document->RemoveReference();
	m_background_context->RemoveReference();
	m_context->RemoveReference();
	m_chat_context->RemoveReference();
}

void UserInterface::render() const {
	// Desk UI.
	if( m_desk_visible ) {
		m_render_target.draw( m_background_shape );
		m_background_context->Render();
		m_context->Render();
	}

	// Chat.
	if( m_chat_prompt_document->IsVisible() ) {
		m_chat_context->Render();
	}
}

bool UserInterface::is_mouse_pointer_needed() const {
	return
		m_desk_visible ||
		m_chat_prompt_document->IsVisible()
	;
}

void UserInterface::handle_event( const sf::Event& event ) {
	m_last_event_consumed = false;

	// If chat prompt is visible, delegate all events to it.
	if( m_chat_prompt_document->IsVisible() ) {
		// ESC however closes the prompt.
		if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape ) {
			m_chat_prompt_document->Show( Rocket::Core::ElementDocument::NONE );
			m_chat_prompt_document->Hide();

			m_last_event_consumed = true;
			return;
		}

		RocketEventDispatcher::dispatch_event( event, *m_chat_context );
		return;
	}

	// The following events only concern windows except chat prompt.

	if( event.type == sf::Event::KeyPressed ) {
		Controls::Action action = m_controls.get_key_action( event.key.code );

		switch( action ) {
			case Controls::DESK:
				// Toggle desk.
				m_desk_visible = !m_desk_visible;

				m_last_event_consumed = true;
				return;

			case Controls::CHAT:
				// Show chat and consume event.
				m_chat_prompt_document->Show( Rocket::Core::ElementDocument::MODAL );

				m_last_event_consumed = true;
				return;

			default:
				break;
		}

		// Do desk events.
		if( m_desk_visible ) {
			// Check for I/J/C keys.
			switch( event.key.code ) {
				case sf::Keyboard::J:
					// Toggle journal.
					if( m_journal_window_document->IsVisible() ) {
						m_journal_window_document->Hide();
					}
					else {
						m_journal_window_document->Show();
					}

					m_last_event_consumed = true;
					return;

				default:
					break;
			}
		}
	}

	// Dispatch event to desk if visible.
	if( m_desk_visible ) {
		RocketEventDispatcher::dispatch_event( event, *m_context );

		m_last_event_consumed = true;
	}
}

JournalWindowController& UserInterface::get_journal_controller() {
	return *m_journal_controller;
}

bool UserInterface::is_consuming_events() const {
	return
		m_last_event_consumed ||
		m_chat_prompt_document->IsVisible() ||
		m_desk_visible
	;
}
