#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>

class JournalWindowController;
class Controls;

namespace Rocket {
namespace Core {
class Context;
class ElementDocument;
}
}

namespace sf {
class RenderTarget;
class Event;
}

/** User interface.
 */
class UserInterface {
	public:
		/** Ctor.
		 * @param render_target Render target.
		 * @param controls Controls.
		 */
		UserInterface(
			sf::RenderTarget& render_target,
			const Controls& controls
		);

		/** Dtor.
		 */
		~UserInterface();

		/** Render.
		 */
		void render() const;

		/** Check if mouse pointer is needed.
		 * It's needed when any window is being displayed.
		 * @return true if needed.
		 */
		bool is_mouse_pointer_needed() const;

		/** Check if desk is visible.
		 * @return true if visible.
		 */
		bool is_desk_visible() const;

		/** Handle event.
		 * @param event Event.
		 */
		void handle_event( const sf::Event& event );

		/** Get journal controller.
		 * @return Journal controller.
		 */
		JournalWindowController& get_journal_controller();

		/** Check if user interface wants to consume events.
		 * @return true if events shall be consumed.
		 */
		bool is_consuming_events() const;

	private:
		sf::RectangleShape m_background_shape;
		std::unique_ptr<JournalWindowController> m_journal_controller;
		sf::RenderTarget& m_render_target;
		const Controls& m_controls;
		Rocket::Core::Context* m_context;
		Rocket::Core::Context* m_background_context;
		Rocket::Core::Context* m_chat_context;
		Rocket::Core::ElementDocument* m_journal_window_document;
		Rocket::Core::ElementDocument* m_toolbar_document;
		Rocket::Core::ElementDocument* m_chat_prompt_document;

		bool m_desk_visible;
		bool m_last_event_consumed;
};
