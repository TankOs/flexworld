#pragma once

#include <FWMS/Reader.hpp>

class Controls;

/** Movement reader.
 * Reader responsible for interpreting inputs (keys/mouse) and generating
 * messages that specify actions (being triggered by inputs) regarding
 * movement.
 *
 * Listens for:
 *   * key_press
 *   * key_release
 *   * button_press
 *   * button_release
 *   * mouse_move
 *
 * Emits:
 *   * walk: vector (sf::Vector2f, x = strafe, y = forward)
 */
class MovementReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		MovementReader();

		/** Set controls.
		 * @param controls Controls.
		 */
		void set_controls( const Controls& controls );

	private:
		void handle_message( const ms::Message& message );

		const Controls* m_controls;
		bool m_walk_forward;
		bool m_walk_backward;
		bool m_strafe_left;
		bool m_strafe_right;
};
