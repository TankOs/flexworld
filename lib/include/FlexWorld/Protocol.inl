#include <cassert>

/// @cond NEVER

namespace flex {

// Impl for end of typelist dispatch.
template <class Org, class MessageIDType, class DataType>
template <class Handler>
std::size_t ProtocolImpl<tpl::None, Org, MessageIDType, DataType>::dispatch( MessageID id, const Buffer& /*buffer*/, Handler& /*handler*/ ) {
	std::string err_msg = "Message ID unknown: ";
	err_msg += static_cast<int>( id );
	throw UnknownMessageIDException( err_msg );

	return 0;
}

template <class MessageTypelist, class Org, class MessageIDType, class DataType>
template <class Handler>
inline std::size_t ProtocolImpl<MessageTypelist, Org, MessageIDType, DataType>::dispatch( MessageID id, const Buffer& buffer, Handler& handler ) {
	if( id == tpl::IndexOf<typename MessageTypelist::Head, Org>::RESULT ) {
		typename MessageTypelist::Head message;

		std::size_t eaten = message.deserialize( buffer );

		handler.handle_message( message );
		return eaten;
	}

	return ProtocolImpl<typename MessageTypelist::Tail, Org, MessageIDType, DataType>::dispatch( id, buffer, handler );
}

template <class MessageTypelist, class MessageIDType, class DataType>
template <class Handler>
std::size_t Protocol<MessageTypelist, MessageIDType, DataType>::dispatch( MessageID id, const Buffer& buffer, Handler& handler ) {
	assert( id != INVALID_MESSAGE_ID );
	return ProtocolImpl<MessageTypelist, MessageTypelist, MessageIDType, DataType>::dispatch( id, buffer, handler );
}

}

/// @endcond NEVER
