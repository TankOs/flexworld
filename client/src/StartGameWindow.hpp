#pragma once

#include <FlexWorld/GameMode.hpp>

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Signal.hpp>

#include <vector>

/** Start game window.
 */
class StartGameWindow : public sfg::Window {
	public:
		typedef sfg::SharedPtr<StartGameWindow> Ptr; ///< Shared pointer.
		typedef sfg::SharedPtr<const StartGameWindow> PtrConst; ///< Shared pointer to const.

		/** Create window.
		 * @return Window.
		 */
		static Ptr Create();

		/** Add game mode to combo box.
		 * @param game_mode Game mode.
		 */
		void add_game_mode( const flex::GameMode& game_mode );

		sfg::Signal OnAccept; ///< Fired when OK clicked.
		sfg::Signal OnReject; ///< Fired when cancel clicked.

	private:
		typedef std::vector<flex::GameMode> GameModeVector;

		StartGameWindow();

		GameModeVector m_game_modes;

		sfg::ComboBox::Ptr m_game_mode_combo;
		sfg::Entry::Ptr m_password_entry;
		sfg::CheckButton::Ptr m_public_check;
		sfg::Scale::Ptr m_max_players_scale;
		sfg::Label::Ptr m_max_players_label;

		void on_ok_click();
		void on_cancel_click();

		void on_max_players_change();
};
