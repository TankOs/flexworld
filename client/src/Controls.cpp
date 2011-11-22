#include "Controls.hpp"

Controls::Controls() {
}

void Controls::clear() {
	m_keys.clear();
	m_buttons.clear();
}

void Controls::map_key( int key, Action action ) {
	unmap_action( action );
	m_keys[key] = action;
}

void Controls::map_button( int button, Action action ) {
	unmap_action( action );
	m_buttons[button] = action;
}

void Controls::unmap_action( Action action ) {
	// Search for mapped key.
	ControlMap::iterator key_iter( m_keys.begin() );
	ControlMap::iterator key_iter_end( m_keys.end() );

	for( ; key_iter != key_iter_end; ++key_iter ) {
		if( key_iter->second == action ) {
			m_keys.erase( key_iter );
			return;
		}
	}

	// Search for mapped mouse button.
	ControlMap::iterator button_iter( m_buttons.begin() );
	ControlMap::iterator button_iter_end( m_buttons.end() );

	for( ; button_iter != button_iter_end; ++button_iter ) {
		if( button_iter->second == action ) {
			m_buttons.erase( button_iter );
			return;
		}
	}
}

Controls::Action Controls::get_key_action( int key ) const {
	ControlMap::const_iterator iter( m_keys.find( key ) );
	return iter == m_keys.end() ? UNMAPPED : iter->second;
}

Controls::Action Controls::get_button_action( int button ) const {
	ControlMap::const_iterator iter( m_buttons.find( button ) );
	return iter == m_buttons.end() ? UNMAPPED : iter->second;
}

Controls::ControlMap::const_iterator Controls::keys_begin() const {
	return m_keys.begin();
}

Controls::ControlMap::const_iterator Controls::keys_end() const {
	return m_keys.end();
}

Controls::ControlMap::const_iterator Controls::buttons_begin() const {
	return m_buttons.begin();
}

Controls::ControlMap::const_iterator Controls::buttons_end() const {
	return m_buttons.end();
}

void Controls::unmap_key( int key ) {
	m_keys.erase( key );
}

void Controls::unmap_button( int button ) {
	m_buttons.erase( button );
}
