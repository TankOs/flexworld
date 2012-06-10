#pragma once

#include "State.hpp"
#include "OptionsWindow.hpp"
#include "StartGameWindow.hpp"

#include <Rocket/Core.h>
#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>

class OptionsDocumentController;

namespace Rocket {
namespace Core {
class Context;
class ElementDocument;
}
}

/** Menu state.
 */
class MenuState : public State, public Rocket::Core::EventListener {
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

		void ProcessEvent( Rocket::Core::Event& event );

		void on_insta_click();
		void on_start_game_click();
		void on_start_game_accept();
		void on_start_game_reject();
		void on_options_click();
		void on_options_accept();
		void on_options_reject();
		void on_quit_click();

		sfg::Desktop m_desktop;
		sfg::Window::Ptr m_window;
		sfg::Button::Ptr m_insta_button;
		sfg::Button::Ptr m_start_game_button;
		sfg::Button::Ptr m_join_game_button;
		sfg::Label::Ptr m_settings_hint_label;

		OptionsWindow::Ptr m_options_window;
		StartGameWindow::Ptr m_start_game_window;

		sf::VertexArray m_background_varray;
		sf::Texture m_background_texture;

		Rocket::Core::Context* m_rocket_context;
		Rocket::Core::ElementDocument* m_options_document;

		std::unique_ptr<OptionsDocumentController> m_options_controller;

		sf::Music m_music;
};
