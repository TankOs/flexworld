#include "MovementReader.hpp"
#include "Controls.hpp"
#include "Shared.hpp"

#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <FWU/Math.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <iostream>

static const ms::HashValue KEY_PRESS_ID = ms::string_hash( "key_press" );
static const ms::HashValue KEY_RELEASE_ID = ms::string_hash( "key_release" );
static const ms::HashValue BUTTON_PRESS_ID = ms::string_hash( "button_press" );
static const ms::HashValue BUTTON_RELEASE_ID = ms::string_hash( "button_release" );
static const ms::HashValue KEY_ID = ms::string_hash( "key" );
static const ms::HashValue BUTTON_ID = ms::string_hash( "button" );
static const ms::HashValue WALK_ID = ms::string_hash( "walk" );
static const ms::HashValue VECTOR_ID = ms::string_hash( "vector" );
static const ms::HashValue MOUSE_MOVE_DELTA_ID = ms::string_hash( "mouse_move_delta" );
static const ms::HashValue DELTA_ID = ms::string_hash( "delta" );
static const ms::HashValue MOUSELOOK_ID = ms::string_hash( "mouselook" );

MovementReader::MovementReader() :
	ms::Reader(),
	m_controls( nullptr ),
	m_walk_forward( false ),
	m_walk_backward( false ),
	m_strafe_left( false ),
	m_strafe_right( false )
{
}

void MovementReader::handle_message( const ms::Message& message ) {
	assert( get_router() != nullptr );
	assert( m_controls != nullptr );

	ms::HashValue id = message.get_id();
	Controls::Action action = Controls::UNMAPPED;
	bool execute = false;

	if( id == MOUSE_MOVE_DELTA_ID ) {
		auto* delta = message.find_property<sf::Vector2i>( DELTA_ID );

		assert( delta != nullptr );

		// Transform mouse delta into mouselook control vector.
		sf::Vector2f mouselook_control{
			static_cast<float>( delta->x ),
			static_cast<float>( delta->y )
		};

		float length = util::length( mouselook_control );

		if( length > 0.0f ) {
			util::normalize( mouselook_control );
			mouselook_control *= (std::min( 30.0f, length ) / 30.0f) * get_shared().user_settings.get_controls().get_mouse_sensitivity();

			if( get_shared().user_settings.get_controls().is_mouse_inverted() ) {
				mouselook_control.y *= -1.0f;
			}
		}

		// Notify readers of mouselook action.
		auto msg = std::make_shared<ms::Message>( MOUSELOOK_ID );
		msg->set_property( VECTOR_ID, mouselook_control );
		get_router()->enqueue_message( msg );
	}
	else if( id == KEY_PRESS_ID ) {
		auto* key = message.find_property<sf::Keyboard::Key>( KEY_ID );

		if( key != nullptr ) {
			action = m_controls->get_key_action( *key );
		}

		execute = true;
	}
	else if( id == KEY_RELEASE_ID ) {
		auto* key = message.find_property<sf::Keyboard::Key>( KEY_ID );

		if( key != nullptr ) {
			action = m_controls->get_key_action( *key );
		}

		execute = false;
	}

	if( action != Controls::UNMAPPED ) {
		bool update_walk_vector = false;

		switch( action ) {
			case Controls::WALK_FORWARD:
				m_walk_forward = execute;
				update_walk_vector = true;
				break;

			case Controls::WALK_BACKWARD:
				m_walk_backward = execute;
				update_walk_vector = true;
				break;

			case Controls::STRAFE_LEFT:
				m_strafe_left = execute;
				update_walk_vector = true;
				break;

			case Controls::STRAFE_RIGHT:
				m_strafe_right = execute;
				update_walk_vector = true;
				break;

			default:
				std::cout << "MovementReader: WARNING: Unhandled action: " << action << std::endl;
				break;
		}

		if( update_walk_vector ) {
			sf::Vector2f walk_vector(
				(m_strafe_left ? -1.0f : 0.0f) + (m_strafe_right ? 1.0f : 0.0f ),
				(m_walk_forward ? 1.0f : 0.0f) + (m_walk_backward ? -1.0f : 0.0f )
			);

			auto walk_message = std::make_shared<ms::Message>( WALK_ID );
			walk_message->set_property( VECTOR_ID, walk_vector );

			get_router()->enqueue_message( walk_message );

			std::cout << "MovementReader: Walk vector updated to " << walk_vector.x << ", " << walk_vector.y << std::endl;
		}
	}
}

void MovementReader::set_controls( const Controls& controls ) {
	m_controls = &controls;
}
