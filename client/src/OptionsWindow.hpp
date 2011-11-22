#pragma once

#include "Controls.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Signal.hpp>

/** Options window.
 */
class OptionsWindow : public sfg::Window {
	public:
		typedef std::shared_ptr<OptionsWindow> Ptr; ///< Shared pointer.
		typedef std::shared_ptr<const OptionsWindow> PtrConst; ///< Shared pointer to const.

		/** Create window.
		 * @return Window.
		 */
		static Ptr Create();

		/** Check if event was processed.
		 * @return true if eaten.
		 */
		bool is_event_processed() const;

		void HandleEvent( const sf::Event& event );

		sfg::Signal OnAccept; ///< Fired when OK clicked.
		sfg::Signal OnReject; ///< Fired when cancel clicked.

	private:
		typedef std::map<sfg::Button::Ptr, Controls::Action> ButtonActionMap;
		typedef std::map<Controls::Action, sfg::Button::Ptr> ActionButtonMap;

		OptionsWindow();

		void show_page( sfg::Widget::PtrConst page );
		void refresh_action_button_labels();

		void on_account_click();
		void on_controls_click();

		void on_ok_click();
		void on_cancel_click();

		void on_action_button_click();

		bool m_event_processed;
		Controls::Action m_next_action;
		Controls m_controls;
		sfg::Button::Ptr m_active_action_button;

		ButtonActionMap m_button_actions;
		ActionButtonMap m_action_buttons;

		sfg::Box::Ptr m_account_page_box;
		sfg::Entry::Ptr m_username_entry;
		sfg::Entry::Ptr m_serial_entry;

		sfg::Box::Ptr m_controls_page_box;
		sfg::Label::Ptr m_waiting_for_input_label;
};
