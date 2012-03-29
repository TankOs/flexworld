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
		void set_sender( const std::string& sender );

		/** Get sender.
		 * @return Sender.
		 */
		const std::string& get_sender() const;

		/** Set target.
		 * @param target Target.
		 */
		void set_target( const std::string& target );

		/** Get target.
		 * @return Target.
		 */
		const std::string& get_target() const;

	private:
		sf::String m_message;
		std::string m_sender;
		std::string m_target;
};

}
}
