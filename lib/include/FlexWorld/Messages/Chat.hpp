#pragma once

#include <FlexWorld/Message.hpp>

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

		/** Set channel.
		 * @param channel Channel (empty for default vicinity channel).
		 */
		void set_channel( const std::string& channel );

		/** Get channel.
		 * @return Channel (empty means default vicinity channel).
		 */
		const std::string& get_channel() const;

		/** Set message.
		 * @param message Message.
		 */
		void set_message( const std::string& message );

		/** Get message.
		 * @return Message.
		 */
		const std::string& get_message() const;

	private:
		std::string m_channel;
		std::string m_message;
};

}
}
