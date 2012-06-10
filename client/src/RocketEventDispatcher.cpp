#include "RocketEventDispatcher.hpp"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Utf.hpp>
#include <Rocket/Core.h>
#include <vector>
#include <iostream> // XXX 

int translate_mouse_button( sf::Mouse::Button button ) {
	int t_button = 0;

	switch( button ) {
		case sf::Mouse::Left:
			t_button = 0;
			break;

		case sf::Mouse::Right:
			t_button = 1;
			break;

		case sf::Mouse::Middle:
			t_button = 2;
			break;

		default:
			t_button = 0;
			break;
	}

	return t_button;
}

int make_key_modifier_state() {
	int state = 0;

	if(
		sf::Keyboard::isKeyPressed( sf::Keyboard::LAlt ) ||
		sf::Keyboard::isKeyPressed( sf::Keyboard::RAlt )
	) {
		state |= Rocket::Core::Input::KM_ALT;
	}

	if(
		sf::Keyboard::isKeyPressed( sf::Keyboard::LControl ) ||
		sf::Keyboard::isKeyPressed( sf::Keyboard::RControl )
	) {
		state |= Rocket::Core::Input::KM_CTRL;
	}

	if(
		sf::Keyboard::isKeyPressed( sf::Keyboard::LShift ) ||
		sf::Keyboard::isKeyPressed( sf::Keyboard::RShift )
	) {
		state |= Rocket::Core::Input::KM_SHIFT;
	}

	return state;
}

Rocket::Core::Input::KeyIdentifier translate_key( sf::Keyboard::Key key ) {
	// Build SFML key -> Rocket key translation table.
	static std::vector<Rocket::Core::Input::KeyIdentifier> rocket_keys;

	if( rocket_keys.size() == 0 ) {
		rocket_keys.resize( sf::Keyboard::KeyCount, Rocket::Core::Input::KI_UNKNOWN );

		rocket_keys[sf::Keyboard::A] = Rocket::Core::Input::KI_A;
		rocket_keys[sf::Keyboard::B] = Rocket::Core::Input::KI_B;
		rocket_keys[sf::Keyboard::C] = Rocket::Core::Input::KI_C;
		rocket_keys[sf::Keyboard::D] = Rocket::Core::Input::KI_D;
		rocket_keys[sf::Keyboard::E] = Rocket::Core::Input::KI_E;
		rocket_keys[sf::Keyboard::F] = Rocket::Core::Input::KI_F;
		rocket_keys[sf::Keyboard::G] = Rocket::Core::Input::KI_G;
		rocket_keys[sf::Keyboard::H] = Rocket::Core::Input::KI_H;
		rocket_keys[sf::Keyboard::I] = Rocket::Core::Input::KI_I;
		rocket_keys[sf::Keyboard::J] = Rocket::Core::Input::KI_J;
		rocket_keys[sf::Keyboard::K] = Rocket::Core::Input::KI_K;
		rocket_keys[sf::Keyboard::L] = Rocket::Core::Input::KI_L;
		rocket_keys[sf::Keyboard::M] = Rocket::Core::Input::KI_M;
		rocket_keys[sf::Keyboard::N] = Rocket::Core::Input::KI_N;
		rocket_keys[sf::Keyboard::O] = Rocket::Core::Input::KI_O;
		rocket_keys[sf::Keyboard::P] = Rocket::Core::Input::KI_P;
		rocket_keys[sf::Keyboard::Q] = Rocket::Core::Input::KI_Q;
		rocket_keys[sf::Keyboard::R] = Rocket::Core::Input::KI_R;
		rocket_keys[sf::Keyboard::S] = Rocket::Core::Input::KI_S;
		rocket_keys[sf::Keyboard::T] = Rocket::Core::Input::KI_T;
		rocket_keys[sf::Keyboard::U] = Rocket::Core::Input::KI_U;
		rocket_keys[sf::Keyboard::V] = Rocket::Core::Input::KI_V;
		rocket_keys[sf::Keyboard::W] = Rocket::Core::Input::KI_W;
		rocket_keys[sf::Keyboard::X] = Rocket::Core::Input::KI_X;
		rocket_keys[sf::Keyboard::Y] = Rocket::Core::Input::KI_Y;
		rocket_keys[sf::Keyboard::Z] = Rocket::Core::Input::KI_Z;
		rocket_keys[sf::Keyboard::Num0] = Rocket::Core::Input::KI_0;
		rocket_keys[sf::Keyboard::Num1] = Rocket::Core::Input::KI_1;
		rocket_keys[sf::Keyboard::Num2] = Rocket::Core::Input::KI_2;
		rocket_keys[sf::Keyboard::Num3] = Rocket::Core::Input::KI_3;
		rocket_keys[sf::Keyboard::Num4] = Rocket::Core::Input::KI_4;
		rocket_keys[sf::Keyboard::Num5] = Rocket::Core::Input::KI_5;
		rocket_keys[sf::Keyboard::Num6] = Rocket::Core::Input::KI_6;
		rocket_keys[sf::Keyboard::Num7] = Rocket::Core::Input::KI_7;
		rocket_keys[sf::Keyboard::Num8] = Rocket::Core::Input::KI_8;
		rocket_keys[sf::Keyboard::Num9] = Rocket::Core::Input::KI_9;
		rocket_keys[sf::Keyboard::Escape] = Rocket::Core::Input::KI_ESCAPE;
		rocket_keys[sf::Keyboard::LControl] = Rocket::Core::Input::KI_LCONTROL;
		rocket_keys[sf::Keyboard::LShift] = Rocket::Core::Input::KI_LSHIFT;
		rocket_keys[sf::Keyboard::LSystem] = Rocket::Core::Input::KI_LWIN;
		rocket_keys[sf::Keyboard::RControl] = Rocket::Core::Input::KI_RCONTROL;
		rocket_keys[sf::Keyboard::RShift] = Rocket::Core::Input::KI_RSHIFT;
		rocket_keys[sf::Keyboard::RSystem] = Rocket::Core::Input::KI_RWIN;
		rocket_keys[sf::Keyboard::Menu] = Rocket::Core::Input::KI_LMENU;
		rocket_keys[sf::Keyboard::LBracket] = Rocket::Core::Input::KI_OEM_4;
		rocket_keys[sf::Keyboard::RBracket] = Rocket::Core::Input::KI_OEM_6;
		rocket_keys[sf::Keyboard::SemiColon] = Rocket::Core::Input::KI_OEM_1;
		rocket_keys[sf::Keyboard::Comma] = Rocket::Core::Input::KI_OEM_COMMA;
		rocket_keys[sf::Keyboard::Period] = Rocket::Core::Input::KI_OEM_PERIOD;
		rocket_keys[sf::Keyboard::Quote] = Rocket::Core::Input::KI_OEM_7;
		rocket_keys[sf::Keyboard::Slash] = Rocket::Core::Input::KI_OEM_2;
		rocket_keys[sf::Keyboard::BackSlash] = Rocket::Core::Input::KI_OEM_5;
		rocket_keys[sf::Keyboard::Tilde] = Rocket::Core::Input::KI_OEM_3;
		rocket_keys[sf::Keyboard::Equal] = Rocket::Core::Input::KI_OEM_PLUS;
		rocket_keys[sf::Keyboard::Dash] = Rocket::Core::Input::KI_OEM_MINUS;
		rocket_keys[sf::Keyboard::Space] = Rocket::Core::Input::KI_SPACE;
		rocket_keys[sf::Keyboard::Return] = Rocket::Core::Input::KI_RETURN;
		rocket_keys[sf::Keyboard::Back] = Rocket::Core::Input::KI_BACK;
		rocket_keys[sf::Keyboard::Tab] = Rocket::Core::Input::KI_TAB;
		rocket_keys[sf::Keyboard::PageUp] = Rocket::Core::Input::KI_PRIOR;
		rocket_keys[sf::Keyboard::PageDown] = Rocket::Core::Input::KI_NEXT;
		rocket_keys[sf::Keyboard::End] = Rocket::Core::Input::KI_END;
		rocket_keys[sf::Keyboard::Home] = Rocket::Core::Input::KI_HOME;
		rocket_keys[sf::Keyboard::Insert] = Rocket::Core::Input::KI_INSERT;
		rocket_keys[sf::Keyboard::Delete] = Rocket::Core::Input::KI_DELETE;
		rocket_keys[sf::Keyboard::Add] = Rocket::Core::Input::KI_ADD;
		rocket_keys[sf::Keyboard::Subtract] = Rocket::Core::Input::KI_SUBTRACT;
		rocket_keys[sf::Keyboard::Multiply] = Rocket::Core::Input::KI_MULTIPLY;
		rocket_keys[sf::Keyboard::Divide] = Rocket::Core::Input::KI_DIVIDE;
		rocket_keys[sf::Keyboard::Left] = Rocket::Core::Input::KI_LEFT;
		rocket_keys[sf::Keyboard::Right] = Rocket::Core::Input::KI_RIGHT;
		rocket_keys[sf::Keyboard::Up] = Rocket::Core::Input::KI_UP;
		rocket_keys[sf::Keyboard::Down] = Rocket::Core::Input::KI_DOWN;
		rocket_keys[sf::Keyboard::Numpad0] = Rocket::Core::Input::KI_NUMPAD0;
		rocket_keys[sf::Keyboard::Numpad1] = Rocket::Core::Input::KI_NUMPAD1;
		rocket_keys[sf::Keyboard::Numpad2] = Rocket::Core::Input::KI_NUMPAD2;
		rocket_keys[sf::Keyboard::Numpad3] = Rocket::Core::Input::KI_NUMPAD3;
		rocket_keys[sf::Keyboard::Numpad4] = Rocket::Core::Input::KI_NUMPAD4;
		rocket_keys[sf::Keyboard::Numpad5] = Rocket::Core::Input::KI_NUMPAD5;
		rocket_keys[sf::Keyboard::Numpad6] = Rocket::Core::Input::KI_NUMPAD6;
		rocket_keys[sf::Keyboard::Numpad7] = Rocket::Core::Input::KI_NUMPAD7;
		rocket_keys[sf::Keyboard::Numpad8] = Rocket::Core::Input::KI_NUMPAD8;
		rocket_keys[sf::Keyboard::Numpad9] = Rocket::Core::Input::KI_NUMPAD9;
		rocket_keys[sf::Keyboard::F1] = Rocket::Core::Input::KI_F1;
		rocket_keys[sf::Keyboard::F2] = Rocket::Core::Input::KI_F2;
		rocket_keys[sf::Keyboard::F3] = Rocket::Core::Input::KI_F3;
		rocket_keys[sf::Keyboard::F4] = Rocket::Core::Input::KI_F4;
		rocket_keys[sf::Keyboard::F5] = Rocket::Core::Input::KI_F5;
		rocket_keys[sf::Keyboard::F6] = Rocket::Core::Input::KI_F6;
		rocket_keys[sf::Keyboard::F7] = Rocket::Core::Input::KI_F7;
		rocket_keys[sf::Keyboard::F8] = Rocket::Core::Input::KI_F8;
		rocket_keys[sf::Keyboard::F9] = Rocket::Core::Input::KI_F9;
		rocket_keys[sf::Keyboard::F10] = Rocket::Core::Input::KI_F10;
		rocket_keys[sf::Keyboard::F11] = Rocket::Core::Input::KI_F11;
		rocket_keys[sf::Keyboard::F12] = Rocket::Core::Input::KI_F12;
		rocket_keys[sf::Keyboard::F13] = Rocket::Core::Input::KI_F13;
		rocket_keys[sf::Keyboard::F14] = Rocket::Core::Input::KI_F14;
		rocket_keys[sf::Keyboard::F15] = Rocket::Core::Input::KI_F15;
		rocket_keys[sf::Keyboard::Pause] = Rocket::Core::Input::KI_PAUSE;
	}

	if( key >= rocket_keys.size() ) {
		return Rocket::Core::Input::KI_UNKNOWN;
	}

	return rocket_keys[key];
}

void RocketEventDispatcher::dispatch_event( const sf::Event& event, Rocket::Core::Context& context ) {
	// Translate SFML event to libRocket event.
	if( event.type == sf::Event::MouseMoved ) {
		context.ProcessMouseMove( event.mouseMove.x, event.mouseMove.y, make_key_modifier_state() );
	}
	else if( event.type == sf::Event::MouseButtonPressed ) {
		context.ProcessMouseButtonDown( translate_mouse_button( event.mouseButton.button ), make_key_modifier_state() );
	}
	else if( event.type == sf::Event::MouseButtonReleased ) {
		context.ProcessMouseButtonUp( translate_mouse_button( event.mouseButton.button ), make_key_modifier_state() );
	}
	else if( event.type == sf::Event::MouseWheelMoved ) {
		context.ProcessMouseWheel( -event.mouseWheel.delta, make_key_modifier_state() );
	}
	else if( event.type == sf::Event::TextEntered ) {
		// Only process codes >= 0x20, otherwise we'd process control characters.
		sf::Uint32 ch = event.text.unicode;

		if( ch >= 0x20 ) {
			// Convert UTF-32 to UTF-16 (UCS-2 is needed by libRocket).
			sf::Uint16 rocket_ch = 0;

			sf::Utf<16>::encode( ch, &rocket_ch );

			context.ProcessTextInput( rocket_ch );
		}
	}
	else if( event.type == sf::Event::KeyPressed ) {
		context.ProcessKeyDown( translate_key( event.key.code ), make_key_modifier_state() );
	}
	else if( event.type == sf::Event::KeyReleased ) {
		context.ProcessKeyUp( translate_key( event.key.code ), make_key_modifier_state() );
	}
}
