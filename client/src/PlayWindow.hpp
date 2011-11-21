#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Signal.hpp>

/** Options window.
 */
class PlayWindow : public sfg::Window {
	public:
		typedef std::shared_ptr<PlayWindow> Ptr; ///< Shared pointer.
		typedef std::shared_ptr<const PlayWindow> PtrConst; ///< Shared pointer to const.

		/** Create window.
		 * @return Window.
		 */
		static Ptr Create();

		sfg::Signal OnAccept; ///< Fired when OK clicked.
		sfg::Signal OnReject; ///< Fired when cancel clicked.

	private:
		PlayWindow();

		sfg::Entry::Ptr m_game_mode_entry;
		sfg::Entry::Ptr m_savegame_entry;
		sfg::Entry::Ptr m_password_entry;
		sfg::CheckButton::Ptr m_public_check;
		sfg::Scale::Ptr m_max_players_scale;
		sfg::Label::Ptr m_max_players_label;

		void on_ok_click();
		void on_cancel_click();

		void on_max_players_change();
};
