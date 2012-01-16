#include <cassert>

/// @cond NEVER

namespace flex {

// Impl for end of typelist dispatch.
template <class Org>
template <class Handler>
std::size_t ProtocolImpl<tpl::None, Org>::dispatch( MessageID id, const Buffer& /*buffer*/, Handler& /*handler*/, ConnectionID /*sender*/ ) {
	std::string err_msg = "Message ID unknown: ";
	err_msg += static_cast<int>( id );
	throw UnknownMessageIDException( err_msg );

	return 0;
}

template <class MessageTypelist, class Org>
template <class Handler>
inline std::size_t ProtocolImpl<MessageTypelist, Org>::dispatch( MessageID id, const Buffer& buffer, Handler& handler, ConnectionID sender ) {
	if( id == tpl::IndexOf<typename MessageTypelist::Head, Org>::RESULT ) {
		typename MessageTypelist::Head message;

		std::size_t eaten = message.deserialize( buffer );

		handler.handle_message( message, sender );
		return eaten;
	}

	return ProtocolImpl<typename MessageTypelist::Tail, Org>::dispatch( id, buffer, handler, sender );
}

template <class MessageTypelist>
template <class Handler>
std::size_t Protocol<MessageTypelist>::dispatch( MessageID id, const Buffer& buffer, Handler& handler, ConnectionID sender ) {
	assert( id != INVALID_MESSAGE_ID );
	return ProtocolImpl<MessageTypelist, MessageTypelist>::dispatch( id, buffer, handler, sender );
}

}

/// @endcond NEVER
