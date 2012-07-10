#include "UserInterface.hpp"
#include "ChatController.hpp"
#include "ContainerManager.hpp"
#include "RocketEventDispatcher.hpp"
#include "Controls.hpp"

#include <FlexWorld/Config.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/String.hpp>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <cassert>
#include <iostream> // XXX 

UserInterface::UserInterface(
	sf::RenderTarget& render_target,
	const Controls& controls,
	ResourceManager& resource_manager
) :
	m_background_shape( sf::Vector2f( static_cast<float>( render_target.getSize().x ), static_cast<float>( render_target.getSize().y ) ) ),
	m_render_target( render_target ),
	m_controls( controls ),
	m_resource_manager( resource_manager ),
	m_context( 
		Rocket::Core::CreateContext(
			"desk",
			Rocket::Core::Vector2i(
				render_target.getSize().x,
				render_target.getSize().y
			)
		)
	),
	m_chat_context( 
		Rocket::Core::CreateContext(
			"default",
			Rocket::Core::Vector2i(
				render_target.getSize().x,
				render_target.getSize().y
			)
		)
	),
	m_chat_document( nullptr ),
	m_desk_visible( false ),
	m_last_event_consumed( false ),
	m_consume_next_chat_event( false )
{
	// Load documents.
	m_chat_document = m_chat_context->LoadDocument(
		(fw::ROOT_DATA_DIRECTORY + std::string( "local/gui/chat.rml" )).c_str()
	);
	assert( m_chat_document );

	// Prepare controllers.
	m_chat_controller.reset( new ChatController( m_chat_document ) );
	m_chat_controller->on_send_click = std::bind( &UserInterface::on_chat_send_click, this );

	m_container_manager.reset( new ContainerManager( *m_context, m_resource_manager ) );

	// Background shape.
	m_background_shape.setFillColor( sf::Color( 0, 0, 0, 130 ) );

	// Load sounds.
	m_open_book_sound_buffer.loadFromFile( fw::ROOT_DATA_DIRECTORY + std::string( "local/sfx/page0.wav" ) );
	m_close_book_sound_buffer.loadFromFile( fw::ROOT_DATA_DIRECTORY + std::string( "local/sfx/page1.wav" ) );

	m_open_book_sound = sf::Sound( m_open_book_sound_buffer );
	m_close_book_sound = sf::Sound( m_close_book_sound_buffer );
}

UserInterface::~UserInterface() {
	m_chat_document->RemoveReference();
	m_context->RemoveReference();
	m_chat_context->RemoveReference();
}

void UserInterface::render() const {
	// Desk UI.
	if( m_desk_visible ) {
		m_render_target.draw( m_background_shape );
		m_context->Render();
	}

	// Chat.
	if( m_chat_document->IsVisible() ) {
		m_chat_context->Render();
	}
}

bool UserInterface::is_mouse_pointer_needed() const {
	return
		m_desk_visible ||
		m_chat_document->IsVisible()
	;
}

void UserInterface::handle_event( const sf::Event& event ) {
	m_last_event_consumed = false;

	// If chat prompt is visible, delegate all events to it.
	if( m_chat_document->IsVisible() ) {
		// ESC however closes the prompt.
		if( event.type == sf::Event::KeyPressed ) {
			if( event.key.code == sf::Keyboard::Escape ) {
				m_chat_document->Show( Rocket::Core::ElementDocument::NONE );
				m_chat_document->Hide();

				m_close_book_sound.play();
				m_last_event_consumed = true;
				return;
			}
			else if( event.key.code == sf::Keyboard::Return ) {
				on_chat_send_click();
				return;
			}
		}

		if( !m_consume_next_chat_event ) {
			RocketEventDispatcher::dispatch_event( event, *m_chat_context );
		}

		m_consume_next_chat_event = false;
		return;
	}

	// The following events only concern windows except chat prompt.
	if( event.type == sf::Event::KeyPressed ) {
		// Close desk?
		if( event.key.code == sf::Keyboard::Escape ) {
			if( m_desk_visible ) {
				m_desk_visible = false;
				m_close_book_sound.play();
				m_last_event_consumed = true;
				return;
			}
		}

		Controls::Action action = m_controls.get_key_action( event.key.code );

		switch( action ) {
			case Controls::DESK:
				// Toggle desk.
				m_desk_visible = !m_desk_visible;

				if( m_desk_visible ) {
					m_open_book_sound.play();
				}
				else {
					m_close_book_sound.play();
				}

				m_last_event_consumed = true;
				return;

			case Controls::CHAT:
				{
					// Show chat and consume event.
					m_chat_document->Show( Rocket::Core::ElementDocument::MODAL );

					// Focus message box and clear it.
					m_chat_controller->set_message( "" );
					m_chat_controller->focus_message_box();

					m_open_book_sound.play();
					m_last_event_consumed = true;
					m_consume_next_chat_event = true;
				}
				return;

			default:
				break;
		}
	}

	// Dispatch event to desk if visible.
	if( m_desk_visible ) {
		RocketEventDispatcher::dispatch_event( event, *m_context );

		m_last_event_consumed = true;
	}
}

bool UserInterface::is_consuming_events() const {
	return
		m_last_event_consumed ||
		m_chat_document->IsVisible() ||
		m_desk_visible
	;
}

void UserInterface::update( const sf::Time& /*delta*/ ) {
	m_context->Update();
	m_chat_context->Update();
}

void UserInterface::on_chat_send_click() {
	// Get message.
	sf::String message = m_chat_controller->get_message();

	// Give observer.
	if( message.getSize() > 0 && on_chat_message ) {
		on_chat_message( message );
	}

	// Close chat window only if user doesn't hold CTRL.
	if(
		!sf::Keyboard::isKeyPressed( sf::Keyboard::LControl ) &&
		!sf::Keyboard::isKeyPressed( sf::Keyboard::RControl )
	) {
		m_chat_document->Show( Rocket::Core::ElementDocument::NONE );
		m_chat_document->Hide();
		m_close_book_sound.play();
	}

	m_chat_controller->set_message( "" );
	m_chat_controller->focus_message_box();

	m_last_event_consumed = true;
}

ChatController& UserInterface::get_chat_controller() {
	return *m_chat_controller;
}

ContainerManager& UserInterface::get_container_manager() {
	return *m_container_manager;
}
