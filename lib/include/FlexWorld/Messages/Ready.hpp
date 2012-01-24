#pragma once

#include <FlexWorld/Message.hpp>

#include <string>

namespace flex {
namespace msg {

/** Ready network message.
 */
class Ready : public Message {
	public:
		/** Ctor.
		 */
		Ready();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
};

}
}
