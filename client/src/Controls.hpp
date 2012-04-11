#pragma once

#include <map>

/** Key & mouse bindings and other control settings.
 */
class Controls {
	public:
		/** Actions.
		 */
		enum Action {
			UNMAPPED = 0, ///< Unmapped.
			WALK_FORWARD, ///< Walk forward.
			WALK_BACKWARD, ///< Walk backward.
			STRAFE_LEFT, ///< Strafe left.
			STRAFE_RIGHT, ///< Strafe right.
			JUMP, ///< Jump.
			CROUCH, ///< Crouch.
			USE, ///< Use.
			DROP, ///< Drop.
			PRIMARY_ACTION, ///< Primary action.
			SECONDARY_ACTION, ///< Secondary action.
			INVENTORY, ///< Inventory.
			CHAT, ///< Chat.
			NUM_ACTIONS ///< Total number of actions.
		};

		typedef std::map<const int, Action> ControlMap; ///< Key/Button -> Action bindings.

		/** Ctor.
		 */
		Controls();

		/** Clear bindings and reset settings.
		 */
		void clear();

		/** Map key to action.
		 * @param key Key.
		 * @param action Action.
		 */
		void map_key( int key, Action action );

		/** Map mouse button to action.
		 * @param button Button.
		 * @param action Action.
		 */
		void map_button( int button, Action action );

		/** Unmap all keys and buttons for a specific action.
		 * @param action Action.
		 */
		void unmap_action( Action action );

		/** Unmap key binding.
		 * @param key Key.
		 */
		void unmap_key( int key );

		/** Unmap button binding.
		 * @param button Button.
		 */
		void unmap_button( int button );

		/** Get mapped action for a key.
		 * @param key Key.
		 * @return Action (incl. UNMAPPED).
		 */
		Action get_key_action( int key ) const;

		/** Get mapped action for a mouse button.
		 * @param button Mouse button.
		 * @return Action (incl. UNMAPPED).
		 */
		Action get_button_action( int button ) const;

		/** Begin iterator for key bindings.
		 * @return Begin iterator.
		 */
		ControlMap::const_iterator keys_begin() const;

		/** End iterator for key bindings.
		 * @return End iterator.
		 */
		ControlMap::const_iterator keys_end() const;

		/** Begin iterator for button bindings.
		 * @return Begin iterator.
		 */
		ControlMap::const_iterator buttons_begin() const;

		/** End iterator for button bindings.
		 * @return End iterator.
		 */
		ControlMap::const_iterator buttons_end() const;

		/** Set if mouse is inverted.
		 * @param inverted True to invert.
		 */
		void set_mouse_inverted( bool inverted );

		/** Check if mouse is inverted.
		 * @return true if inverted.
		 */
		bool is_mouse_inverted() const;

		/** Set mouse sensitivity.
		 * @param sens Sensitivity.
		 */
		void set_mouse_sensitivity( float sens );

		/** Get mouse sensitivity.
		 * @return Sensitivity.
		 */
		float get_mouse_sensitivity() const;

	private:
		bool m_mouse_inverted;
		float m_mouse_sensitivity;

		ControlMap m_keys;
		ControlMap m_buttons;
};
