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
		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( uint32_t delta );
		void render() const;

		void on_play_click();
		void on_play_accept();
		void on_play_reject();
		void on_options_click();
		void on_options_accept();
		void on_options_reject();
		void on_quit_click();

		sfg::Desktop m_desktop;
		sfg::Widget::Ptr m_options_window;
		sfg::Widget::Ptr m_play_window;

		sf::Texture m_bg_texture;
		sf::Music m_music;
};
