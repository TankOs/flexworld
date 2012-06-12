#pragma once

#include <SFML/System/String.hpp>
#include <vector>
#include <string>

namespace Rocket {
namespace Core {
class Element;
}
}

/** Controller for the GUI chat window.
 */
class JournalWindowController {
	public:
		/** Ctor.
		 * @param root Root element of the chat window.
		 */
		JournalWindowController( Rocket::Core::Element* root );

		/** Dtor.
		 */
		~JournalWindowController();

		/** Add message.
		 * Channel is created if it doesn't exist.
		 * @param message Message.
		 * @param channel Channel.
		 */
		void add_message( const sf::String& message, const std::string& channel );

	private:
		struct ChannelInfo {
			bool operator==( const std::string& name ) const;

			std::string name;
			Rocket::Core::Element* panel;
		};

		typedef std::vector<ChannelInfo> ChannelArray;

		ChannelArray m_channels;
		Rocket::Core::Element* m_root;
};
