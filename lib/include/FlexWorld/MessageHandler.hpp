#pragma once

#include <FlexWorld/TemplateUtils.hpp>

namespace flex {

/// @cond NEVER
/** Recursive message handler.
 */
template <class TL, class ConnID>
struct MessageHandler : public MessageHandler<typename TL::Tail, ConnID> {
	virtual void handle_message( const typename TL::Head& message, ConnID sender );
};

/** End of recursive message handler.
 */
template <class ConnID>
struct MessageHandler<tpl::None, ConnID> {
};
/// @endcond

}

#include "MessageHandler.inl"
