#pragma once

#include <Rocket/Core/EventListener.h>
#include <functional>

namespace sf {
class String;
}

namespace Rocket {
namespace Core {
class Element;
class Event;
}
namespace Controls {
class ElementFormControlInput;
class ElementFormControlSelect;
}
}

/** Controller for the chat window.
 */
class ChatController : public Rocket::Core::EventListener {
	public:
		/** Ctor.
		 * @param root Chat window root element.
		 */
		ChatController( Rocket::Core::Element* root );

		/** Dtor.
		 */
		~ChatController();

		/** Process Rocket event.
		 * @param event Event.
		 */
		void ProcessEvent( Rocket::Core::Event& event );

		/** Get entered chat message.
		 * @return Message.
		 */
		sf::String get_message() const;

		/** Set message.
		 * @param message Message.
		 */
		void set_message( const sf::String& message );

		/** Add message.
		 * Channel is created if it doesn't exist.
		 * @param message Message.
		 * @param channel Channel.
		 */
		void add_message( const sf::String& message, const std::string& channel );

		/** Focus message box.
		 */
		void focus_message_box();

		std::function<void()> on_send_click; ///< Fired when send button is clicked.

	private:
		struct ChannelInfo {
			bool operator==( const std::string& name ) const;

			std::string name;
			Rocket::Core::Element* panel;
		};

		typedef std::vector<ChannelInfo> ChannelArray;

		ChannelArray m_channels;
		Rocket::Core::Element* m_root;
		Rocket::Core::Element* m_send_element;
		Rocket::Controls::ElementFormControlInput* m_message_element;
};
