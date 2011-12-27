#pragma once

#include "UserSettings.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Signal.hpp>

/** Options window.
 */
class OptionsWindow : public sfg::Window {
	public:
		typedef sfg::SharedPtr<OptionsWindow> Ptr; ///< Shared pointer.
		typedef sfg::SharedPtr<const OptionsWindow> PtrConst; ///< Shared pointer to const.

		/** Create window.
		 * @param user_settings User user_settings, used to fill the widgets.
		 * @return Window.
		 */
		static Ptr Create( const UserSettings& user_settings );

		/** Check if event was processed.
		 * @return true if eaten.
		 */
		bool is_event_processed() const;

		/** Handle event.
		 * @param event Event.
		 */
		void HandleEvent( const sf::Event& event );

		/** Get user settings.
		 * The object is updated when the user clicks "OK".
		 * @return User settings.
		 */
		const UserSettings& get_user_settings() const;

		sfg::Signal OnAccept; ///< Fired when OK clicked.
		sfg::Signal OnReject; ///< Fired when cancel clicked.

	private:
		typedef std::map<sfg::Button::Ptr, Controls::Action> ButtonActionMap;
		typedef std::map<Controls::Action, sfg::Button::Ptr> ActionButtonMap;

		OptionsWindow( const UserSettings& user_settings );

		void show_page( sfg::Widget::Ptr page );
		void refresh_action_button_labels();

		void on_account_click();
		void on_controls_click();

		void on_ok_click();
		void on_cancel_click();

		void on_action_button_click();
		void on_sensitivity_change();

		UserSettings m_user_settings;

		bool m_event_processed;
		Controls::Action m_next_action;

		sfg::Button::Ptr m_active_action_button;

		ButtonActionMap m_button_actions;
		ActionButtonMap m_action_buttons;

		sfg::Box::Ptr m_account_page_box;
		sfg::Entry::Ptr m_username_entry;
		sfg::Entry::Ptr m_serial_entry;

		sfg::Box::Ptr m_controls_page_box;
		sfg::CheckButton::Ptr m_mouse_inverted_check;
		sfg::Scale::Ptr m_mouse_sensitivity_scale;
		sfg::Label::Ptr m_mouse_sensitivity_label;
		sfg::Label::Ptr m_waiting_for_input_label;
};
