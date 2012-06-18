#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <boost/function.hpp>
#include <memory>

class ChatController;
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
class String;
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

		/** Update.
		 * @param delta Time difference between this and the last call.
		 */
		void update( const sf::Time& delta );

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

		/** Check if user interface wants to consume events.
		 * @return true if events shall be consumed.
		 */
		bool is_consuming_events() const;

		/** Get chat controller.
		 * @return Chat controller.
		 */
		ChatController& get_chat_controller();

		boost::function<void( const sf::String& )> on_chat_message; ///< Fired when user wants to send chat message.

	private:
		void on_chat_send_click();

		sf::SoundBuffer m_open_book_sound_buffer;
		sf::SoundBuffer m_close_book_sound_buffer;
		sf::Sound m_open_book_sound;
		sf::Sound m_close_book_sound;
		sf::RectangleShape m_background_shape;
		std::unique_ptr<ChatController> m_chat_controller;

		sf::RenderTarget& m_render_target;
		const Controls& m_controls;
		Rocket::Core::Context* m_context;
		Rocket::Core::Context* m_chat_context;
		Rocket::Core::ElementDocument* m_chat_document;

		bool m_desk_visible;
		bool m_last_event_consumed;
		bool m_consume_next_chat_event;
};
