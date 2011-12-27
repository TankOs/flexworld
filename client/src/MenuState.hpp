#pragma once

#include "State.hpp"
#include "ColorMorpher.hpp"
#include "OptionsWindow.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Music.hpp>

/** Menu state.
 */
class MenuState : public State {
	public:
		/** Ctor.
		 * @param target Rendering target.
		 */
		MenuState( sf::RenderWindow& target );

	private:
		typedef std::list<sf::Sprite> SpriteList;

		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( uint32_t delta );
		void render() const;

		void morph_sky( const sf::Color& color0, const sf::Color color1, uint32_t time );

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
		sfg::Button::Ptr m_start_game_button;
		sfg::Button::Ptr m_join_game_button;
		sfg::Label::Ptr m_settings_hint_label;

		OptionsWindow::Ptr m_options_window;
		sfg::Widget::Ptr m_start_game_window;

		sf::Texture m_cloud_texture;
		SpriteList m_cloud_sprites;

		sf::Shape m_sky_shape;
		ColorMorpher m_morphers[2];

		sf::Music m_music;
};
