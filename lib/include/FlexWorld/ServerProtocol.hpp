#pragma once

#include <FlexWorld/Protocol.hpp>

#include <string>

namespace flex {

/** ServerProtocol provides handlers for messages and is a factory for creating those.
 */
class ServerProtocol : public Protocol {
	public:
		/** Ctor.
		 */
		ServerProtocol();

		/** Dtor.
		 */
		virtual ~ServerProtocol();

		std::size_t handle_incoming_data( Socket& socket, const Buffer& buffer );

		/** Build login message.
		 * @param username Username.
		 * @param password Password.
		 * @param buffer Destination buffer.
		 */
		static void build_login_message( const std::string& username, const std::string& password, Buffer& buffer );

		/** Login message handler.
		 * @param sender Sender.
		 * @param username Username.
		 * @param password Password.
		 */
		virtual void handle_login_message( Socket& sender, const std::string& username, const std::string& password );

	private:
		/** Message ID.
		 */
		enum MessageID {
			INVALID = 0,
			LOGIN
		};

		template <class T>
		static void pack_byte( const T& value, Buffer& buffer );

		template <class T>
		static void pack_word( const T& value, Buffer& buffer );

		template <class T>
		static void pack_dword( const T& value, Buffer& buffer );

		static void pack_string( const std::string& value, Buffer& buffer );
};

}

#include "ServerProtocol.inl"
