#include "KeyNames.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <map>

std::string get_key_name( int keycode ) {
	static std::map<const int, std::string> key_names;

	if( !key_names.size() ) {
		// Initialize.
		key_names[sf::Keyboard::A] = "A";
		key_names[sf::Keyboard::B] = "B";
		key_names[sf::Keyboard::C] = "C";
		key_names[sf::Keyboard::D] = "D";
		key_names[sf::Keyboard::E] = "E";
		key_names[sf::Keyboard::F] = "F";
		key_names[sf::Keyboard::G] = "G";
		key_names[sf::Keyboard::H] = "H";
		key_names[sf::Keyboard::I] = "I";
		key_names[sf::Keyboard::J] = "J";
		key_names[sf::Keyboard::K] = "K";
		key_names[sf::Keyboard::L] = "L";
		key_names[sf::Keyboard::M] = "M";
		key_names[sf::Keyboard::N] = "N";
		key_names[sf::Keyboard::O] = "O";
		key_names[sf::Keyboard::P] = "P";
		key_names[sf::Keyboard::Q] = "Q";
		key_names[sf::Keyboard::R] = "R";
		key_names[sf::Keyboard::S] = "S";
		key_names[sf::Keyboard::T] = "T";
		key_names[sf::Keyboard::U] = "U";
		key_names[sf::Keyboard::V] = "V";
		key_names[sf::Keyboard::W] = "W";
		key_names[sf::Keyboard::X] = "X";
		key_names[sf::Keyboard::Y] = "Y";
		key_names[sf::Keyboard::Z] = "Z";
		key_names[sf::Keyboard::Num0] = "0";
		key_names[sf::Keyboard::Num1] = "1";
		key_names[sf::Keyboard::Num2] = "2";
		key_names[sf::Keyboard::Num3] = "3";
		key_names[sf::Keyboard::Num4] = "4";
		key_names[sf::Keyboard::Num5] = "5";
		key_names[sf::Keyboard::Num6] = "6";
		key_names[sf::Keyboard::Num7] = "7";
		key_names[sf::Keyboard::Num8] = "8";
		key_names[sf::Keyboard::Num9] = "9";
		key_names[sf::Keyboard::Escape] = "ESC";
		key_names[sf::Keyboard::LControl] = "LCtrl";
		key_names[sf::Keyboard::LShift] = "LShift";
		key_names[sf::Keyboard::LAlt] = "LAlt";
		key_names[sf::Keyboard::LSystem] = "LSuper";
		key_names[sf::Keyboard::RControl] = "RCtrl";
		key_names[sf::Keyboard::RShift] = "RShift";
		key_names[sf::Keyboard::RAlt] = "RAlt";
		key_names[sf::Keyboard::RSystem] = "RSuper";
		key_names[sf::Keyboard::Menu] = "Menu";
		key_names[sf::Keyboard::LBracket] = "{";
		key_names[sf::Keyboard::RBracket] = "}";
		key_names[sf::Keyboard::SemiColon] = ";";
		key_names[sf::Keyboard::Comma] = ",";
		key_names[sf::Keyboard::Period] = ".";
		key_names[sf::Keyboard::Quote] = "\"";
		key_names[sf::Keyboard::Slash] = "/";
		key_names[sf::Keyboard::BackSlash] = "\\";
		key_names[sf::Keyboard::Tilde] = "~";
		key_names[sf::Keyboard::Equal] = "=";
		key_names[sf::Keyboard::Dash] = "-";
		key_names[sf::Keyboard::Space] = "Space";
		key_names[sf::Keyboard::Return] = "Enter";
		key_names[sf::Keyboard::Back] = "Back";
		key_names[sf::Keyboard::Tab] = "Tab";
		key_names[sf::Keyboard::PageUp] = "Page up";
		key_names[sf::Keyboard::PageDown] = "Page down";
		key_names[sf::Keyboard::End] = "End";
		key_names[sf::Keyboard::Home] = "Home";
		key_names[sf::Keyboard::Insert] = "Insert";
		key_names[sf::Keyboard::Delete] = "Delete";
		key_names[sf::Keyboard::Add] = "Add";
		key_names[sf::Keyboard::Subtract] = "Subtract";
		key_names[sf::Keyboard::Multiply] = "Multiply";
		key_names[sf::Keyboard::Divide] = "Divide";
		key_names[sf::Keyboard::Left] = "Left";
		key_names[sf::Keyboard::Right] = "Right";
		key_names[sf::Keyboard::Up] = "Up";
		key_names[sf::Keyboard::Down] = "Down";
		key_names[sf::Keyboard::Numpad0] = "Num0";
		key_names[sf::Keyboard::Numpad1] = "Num1";
		key_names[sf::Keyboard::Numpad2] = "Num2";
		key_names[sf::Keyboard::Numpad3] = "Num3";
		key_names[sf::Keyboard::Numpad4] = "Num4";
		key_names[sf::Keyboard::Numpad5] = "Num5";
		key_names[sf::Keyboard::Numpad6] = "Num6";
		key_names[sf::Keyboard::Numpad7] = "Num7";
		key_names[sf::Keyboard::Numpad8] = "Num8";
		key_names[sf::Keyboard::Numpad9] = "Num9";
		key_names[sf::Keyboard::F1] = "F1";
		key_names[sf::Keyboard::F2] = "F2";
		key_names[sf::Keyboard::F3] = "F3";
		key_names[sf::Keyboard::F4] = "F4";
		key_names[sf::Keyboard::F5] = "F5";
		key_names[sf::Keyboard::F6] = "F6";
		key_names[sf::Keyboard::F7] = "F7";
		key_names[sf::Keyboard::F8] = "F8";
		key_names[sf::Keyboard::F9] = "F9";
		key_names[sf::Keyboard::F10] = "F10";
		key_names[sf::Keyboard::F11] = "F11";
		key_names[sf::Keyboard::F12] = "F12";
		key_names[sf::Keyboard::F13] = "F13";
		key_names[sf::Keyboard::F14] = "F14";
		key_names[sf::Keyboard::F15] = "F15";
		key_names[sf::Keyboard::Pause] = "Pause";
	}

	std::map<const int, std::string>::const_iterator key_iter( key_names.find( keycode ) );

	if( key_iter == key_names.end() ) {
		return "";
	}

	return key_iter->second;
}

std::string get_button_name( int button ) {
	static std::map<const int, std::string> button_names;

	if( !button_names.size() ) {
		// Initialize.
		button_names[sf::Mouse::Left] = "Left button";
		button_names[sf::Mouse::Right] = "Right button";
		button_names[sf::Mouse::Middle] = "Middle button";
		button_names[sf::Mouse::XButton1] = "Back button";
		button_names[sf::Mouse::XButton2] = "Forward button";
	}

	std::map<const int, std::string>::const_iterator button_iter( button_names.find( button ) );

	if( button_iter == button_names.end() ) {
		return "";
	}

	return button_iter->second;
}
