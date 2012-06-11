#pragma once

#include "State.hpp"

#include <Rocket/Core.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>
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
		void on_options_click();
		void on_options_accept();
		void on_options_reject();
		void on_quit_click();

		sf::VertexArray m_background_varray;
		sf::Texture m_background_texture;

		Rocket::Core::Context* m_rocket_context;
		Rocket::Core::ElementDocument* m_options_document;

		std::unique_ptr<OptionsDocumentController> m_options_controller;

		sf::Music m_music;
};
