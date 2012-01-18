#include <cstring>
#include <cassert>

/// @cond NEVER

namespace flex {

template <class MessageTypelist>
const typename Protocol<MessageTypelist>::MessageID Protocol<MessageTypelist>::INVALID_MESSAGE_ID = std::numeric_limits<uint8_t>::max();

// Impl for end of typelist dispatch.
template <class Org>
template <class Handler>
std::size_t ProtocolImpl<tpl::None, Org>::dispatch( MessageID id, const char* /*buffer*/, std::size_t /*buffer_size*/, Handler& /*handler*/, ConnectionID /*sender*/ ) {
	std::string err_msg = "Message ID unknown: ";
	err_msg += static_cast<int>( id );
	throw UnknownMessageIDException( err_msg );

	return 0;
}

template <class MessageTypelist, class Org>
template <class Handler>
inline std::size_t ProtocolImpl<MessageTypelist, Org>::dispatch( MessageID id, const char* buffer, std::size_t buffer_size, Handler& handler, ConnectionID sender ) {
	if( id == tpl::IndexOf<typename MessageTypelist::Head, Org>::RESULT ) {
		typename MessageTypelist::Head message;

		std::size_t eaten = message.deserialize( buffer, buffer_size );

		if( eaten > 0 ) {
			handler.handle_message( message, sender );
		}

		return eaten;
	}

	return ProtocolImpl<typename MessageTypelist::Tail, Org>::dispatch( id, buffer, buffer_size, handler, sender );
}

template <class MessageTypelist>
template <class Handler>
std::size_t Protocol<MessageTypelist>::dispatch( const Buffer& buffer, Handler& handler, ConnectionID sender ) {
	// Peek message ID.
	assert( buffer.size() > 0 );

	// If no message ID present, cancel.
	if( buffer.size() < sizeof( MessageID ) ) {
		throw BogusMessageDataException( "Missing message ID." );
	}

	// If no message data present, cancel.
	if( buffer.size() < sizeof( MessageID ) + 1 ) {
		throw BogusMessageDataException( "Missing message data." );
	}

	MessageID id;
	std::memcpy( &id, &buffer[0], sizeof( MessageID ) );

	if( id == INVALID_MESSAGE_ID ) {
		throw BogusMessageDataException( "Invalid message ID." );
	}

	std::size_t eaten = ProtocolImpl<MessageTypelist, MessageTypelist>::dispatch( id, &buffer.front() + 1, buffer.size() - 1, handler, sender );

	// If message got parsed add size of message ID to eaten bytes count.
	if( eaten > 0 ) {
		eaten += sizeof( MessageID );
	}

	return eaten;
}

template <class MessageTypelist>
template <class MsgType>
void Protocol<MessageTypelist>::serialize_message( const MsgType& message, Buffer& buffer ) {
	// Pack ID.
	MessageID id = tpl::IndexOf<MsgType, MessageTypelist>::RESULT;

	buffer.resize( buffer.size() + sizeof( MessageID ) );
	std::memcpy( &buffer[buffer.size() - sizeof( MessageID )], reinterpret_cast<char*>( &id ), sizeof( MessageID ) );

	// Serialize message.
	message.serialize( buffer );
}

}

/// @endcond NEVER
