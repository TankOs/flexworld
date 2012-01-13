#pragma once

#include <FlexWorld/TemplateUtils.hpp>
#include <FlexWorld/Exception.hpp>

#include <vector>
#include <limits>
#include <cstdint>

namespace flex {

// Skip parsing the impls.
/// @cond NEVER
template <class MessageTypelist, class Org, class MessageIDType, class DataType>
class ProtocolImpl;

template <class Org, class MessageIDType, class DataType>
class ProtocolImpl<tpl::None, Org, MessageIDType, DataType> {
	public:
		typedef typename std::vector<DataType> Buffer; ///< Buffer.
		typedef MessageIDType MessageID; ///< Message ID.

		/** Thrown when message ID unknown.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( UnknownMessageIDException );

	protected:
		template <class Handler>
		static std::size_t dispatch( MessageID id, const Buffer& buffer, Handler& handler );
};

/** FOO
 */
template <class MessageTypelist, class Org, class MessageIDType, class DataType>
class ProtocolImpl : public ProtocolImpl<typename MessageTypelist::Tail, Org, MessageIDType, DataType> {
	public:
		typedef typename std::vector<DataType> Buffer; ///< Buffer.
		typedef MessageIDType MessageID; ///< Message ID.

	protected:
		template <class Handler>
		static std::size_t dispatch( MessageID id, const Buffer& buffer, Handler& handler );
};

/// @endcond

/** Protocol.
 * The protocol includes a dispatcher for dispatching incoming and serialized
 * messages. It also defines some crucial properties of the underlying protocol
 * like message ID type and buffer type.
 *
 * MessageTypelist specifies the typelist that's being used for deserializing
 * and dispatching messages. MessageIDType specifies the type of message IDs.
 * DataType specifies the buffer data type.
 */
template <class MessageTypelist, class MessageIDType, class DataType>
class Protocol : public flex::ProtocolImpl<MessageTypelist, MessageTypelist, MessageIDType, DataType> {
	public:
		typedef typename std::vector<DataType> Buffer; ///< Buffer.
		typedef MessageIDType MessageID; ///< Message ID.
		static const MessageID INVALID_MESSAGE_ID = std::numeric_limits<MessageID>::max(); ///< Invalid message ID.

		/** Dispatch message.
		 * The message will be parsed from the buffer. If parsing succeeds, the
		 * message will be given to the proper method in the given handler.
		 * @param id Message ID.
		 * @param buffer Buffer.
		 * @param handler Handler.
		 * @return Processed bytes (useful for shrinking the buffer).
		 * @throws UnknownMessageIDException when no message is registered for the given ID.
		 * @throws BogusMessageDataException when buffer contains invalid data for the given message ID.
		 */
		template <class Handler>
		static std::size_t dispatch( MessageID id, const Buffer& buffer, Handler& handler );

	private:
};

}

#include "Protocol.inl"
