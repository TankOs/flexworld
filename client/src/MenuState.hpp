#pragma once

#include "State.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Music.hpp>

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

		void on_start_game_click();
		void on_start_game_accept();
		void on_start_game_reject();
		void on_options_click();
		void on_options_accept();
		void on_options_reject();
		void on_quit_click();

		sfg::Desktop m_desktop;
		sfg::Widget::Ptr m_options_window;
		sfg::Widget::Ptr m_start_game_window;

		sf::Texture m_bg_texture;
		sf::Texture m_cloud_texture;
		SpriteList m_cloud_sprites;

		sf::Music m_music;
};
