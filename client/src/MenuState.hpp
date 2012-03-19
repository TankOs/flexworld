#pragma once

#include "State.hpp"
#include "OptionsWindow.hpp"
#include "StartGameWindow.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>

/** Menu state.
 */
class MenuState : public State {
	public:
		/** Ctor.
		 * @param target Rendering target.
		 */
		MenuState( sf::RenderWindow& target );

	private:
		typedef std::vector<sf::Sprite> SpriteVector;

		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( const sf::Time& delta );
		void render() const;

		void on_insta_click();
		void on_start_game_click();
		void on_start_game_accept();
		void on_start_game_reject();
		void on_options_click();
		void on_options_accept();
		void on_options_reject();
		void on_quit_click();

		void check_required_settings();

		sfg::Desktop m_desktop;
		sfg::Window::Ptr m_window;
		sfg::Button::Ptr m_insta_button;
		sfg::Button::Ptr m_start_game_button;
		sfg::Button::Ptr m_join_game_button;
		sfg::Label::Ptr m_settings_hint_label;

		OptionsWindow::Ptr m_options_window;
		StartGameWindow::Ptr m_start_game_window;

		sfg::Widget::Ptr m_sliding_widget;

		bool m_fade_main_menu_out;

		sf::Texture m_cloud_texture;
		SpriteVector m_cloud_sprites;

		sf::VertexArray m_background_varray;
};
