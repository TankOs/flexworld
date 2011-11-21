#pragma once

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

		sfg::Signal OnAccept; ///< Fired when OK clicked.
		sfg::Signal OnReject; ///< Fired when cancel clicked.

	private:
		OptionsWindow();

		void show_page( sfg::Widget::PtrConst page );

		void on_account_click();
		void on_controls_click();

		void on_ok_click();
		void on_cancel_click();

		sfg::Box::Ptr m_account_page_box;
		sfg::Entry::Ptr m_username_entry;
		sfg::Entry::Ptr m_serial_entry;

		sfg::Box::Ptr m_controls_page_box;
};
