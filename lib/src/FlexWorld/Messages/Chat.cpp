#include <FlexWorld/Messages/Chat.hpp>

#include <cassert>

namespace flex {
namespace msg {

Chat::Chat() :
	Message()
{
}

void Chat::set_channel( const std::string& channel ) {
	m_channel = channel;
}

const std::string& Chat::get_channel() const {
	return m_channel;
}

void Chat::set_message( const std::string& message ) {
	m_message = message;
}

const std::string& Chat::get_message() const {
	return m_message;
}

}
}
