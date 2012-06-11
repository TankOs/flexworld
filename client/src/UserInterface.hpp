#pragma once

namespace Rocket {
namespace Core {
class Context;
class ElementDocument;
}
}

namespace sf {
class RenderTarget;
}

/** User interface.
 */
class UserInterface {
	public:
		/** Ctor.
		 * @param render_target Render target.
		 */
		UserInterface( const sf::RenderTarget& render_target );

		/** Dtor.
		 */
		~UserInterface();

		/** Render.
		 */
		void render() const;

	private:
		const sf::RenderTarget& m_render_target;
		Rocket::Core::Context* m_context;

		// Chat window.
		Rocket::Core::ElementDocument* m_chat_window_document;
};
