#pragma once

#include <FlexWorld/Message.hpp>

#include <SFML/System/String.hpp>
#include <string>

namespace flex {
namespace msg {

/** Chat network message.
 */
class Chat : public Message {
	public:
		/** Ctor.
		 */
		Chat();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Set message.
		 * @param message Message.
		 */
		void set_message( const sf::String& message );

		/** Get message.
		 * @return Message.
		 */
		const sf::String& get_message() const;

		/** Set sender.
		 * @param sender Sender.
		 */
		void set_sender( const sf::String& sender );

		/** Get sender.
		 * @return Sender.
		 */
		const sf::String& get_sender() const;

		/** Set channel.
		 * @param channel Channel.
		 */
		void set_channel( const sf::String& channel );

		/** Get channel.
		 * @return Channel.
		 */
		const sf::String& get_channel() const;

	private:
		sf::String m_message;
		sf::String m_channel;
		sf::String m_sender;
};

}
}
