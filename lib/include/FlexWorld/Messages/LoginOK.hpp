#pragma once

#include <FlexWorld/Message.hpp>

#include <string>

namespace flex {
namespace msg {

/** LoginOK network message.
 */
class LoginOK : public Message {
	public:
		/** Ctor.
		 */
		LoginOK();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
};

}
}
