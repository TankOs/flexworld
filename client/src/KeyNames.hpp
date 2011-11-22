#pragma once

#include <string>

/** Get a descriptive name for a key.
 * @param keycode Keycode.
 * @return Name or empty if not defined.
 */
std::string get_key_name( int keycode );

/** Get a descriptive name for a mouse button.
 * @param button Button.
 * @return Name or empty if not defined.
 */
std::string get_button_name( int button );
