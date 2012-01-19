#pragma once

#include <FlexWorld/TemplateUtils.hpp>

namespace flex {

/// @cond NEVER

template <class TL, class ConnID>
struct MessageHandler;

/** Recursive message handler.
 */
template <class TL, class ConnID>
struct MessageHandler : public MessageHandler<typename TL::Tail, ConnID> {
	using MessageHandler<typename TL::Tail, ConnID>::handle_message; // Avoid overwriting.

	virtual void handle_message( const typename TL::Head& message, ConnID sender );
};

/** End of recursive message handler.
 */
template <class ConnID>
struct MessageHandler<tpl::None, ConnID> {
	void handle_message() {}
};
/// @endcond

}

#include "MessageHandler.inl"
