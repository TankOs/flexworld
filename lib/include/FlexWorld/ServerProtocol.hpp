#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Messages/Login.hpp>
#include <FlexWorld/Messages/Chat.hpp>
#include <FlexWorld/TemplateUtils.hpp>

#include <string>

namespace flex {

typedef tpl::Typelist<msg::Login, tpl::Typelist<msg::Chat> > ServerMessageList;
//typedef Protocol<ServerMessageList, uint8_t> ServerProtocol;

#if 0
/** Server-specific protocol.
 */
class ServerProtocol : public Protocol {
	public:

		/** Ctor.
		 */
		ServerProtocol();

		/** Dtor.
		 */
		virtual ~ServerProtocol();

		std::size_t handle_incoming_data( ConnectionID id, const Buffer& buffer );

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
		virtual void handle_login_message( ConnectionID sender, const std::string& username, const std::string& password );

	private:
		template <class T>
		static void pack_byte( const T& value, Buffer& buffer );

		template <class T>
		static void pack_word( const T& value, Buffer& buffer );

		template <class T>
		static void pack_dword( const T& value, Buffer& buffer );

		static void pack_string( const std::string& value, Buffer& buffer );
};
#endif

}

//#include "ServerProtocol.inl"
