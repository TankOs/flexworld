#pragma once

#include <FWMS/Reader.hpp>

namespace ms {
class Message;
}

/** Debug reader for watching the message bus.
 */
class DebugReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		DebugReader();

	private:
		void handle_message( const ms::Message& message );
};
