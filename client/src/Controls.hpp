#pragma once

#include <map>

/** Mapper for keyboard and mouse controls.
 */
class Controls {
	public:
		/** Actions.
		 */
		enum Action {
			UNMAPPED = 0,
			WALK_FORWARD,
			WALK_BACKWARD,
			STRAFE_LEFT,
			STRAFE_RIGHT,
			JUMP,
			CROUCH,
			USE,
			DROP,
			PRIMARY_ATTACK,
			SECONDARY_ATTACK,
			INVENTORY,
			CHAT
		};

		typedef std::map<const int, Action> ControlMap; ///< Key/Button -> Action mappings.

		/** Ctor.
		 */
		Controls();

		/** Clear mappings.
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

		/* Begin iterator for key mappings.
		 * @return Begin iterator.
		 */
		ControlMap::const_iterator keys_begin() const;

		/* End iterator for key mappings.
		 * @return End iterator.
		 */
		ControlMap::const_iterator keys_end() const;

		/* Begin iterator for button mappings.
		 * @return Begin iterator.
		 */
		ControlMap::const_iterator buttons_begin() const;

		/* End iterator for button mappings.
		 * @return End iterator.
		 */
		ControlMap::const_iterator buttons_end() const;

	private:
		ControlMap m_keys;
		ControlMap m_buttons;
};
