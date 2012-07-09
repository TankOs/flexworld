#pragma once

#include <FlexWorld/MessageHandler.hpp>
#include <FlexWorld/ServerProtocol.hpp>

#include <boost/asio/ip/tcp.hpp>

namespace fw {

/** Client.
 *
 * The client also uses connection IDs like the Server class, however since
 * this class only supports one concurrent connection it will always be 0.
 */
class Client {
	public:
		typedef ServerProtocol::ConnectionID ConnectionID; ///< Connection ID.

		/** Handler interface.
		 */
		struct Handler : public MessageHandler<ServerMessageList, ConnectionID> {
			using MessageHandler<ServerMessageList, ConnectionID>::handle_message;

			/** Handle incoming connection.
			 * @param id ID.
			 */
			virtual void handle_connect( ConnectionID id );

			/** Handle disconnection.
			 * @param id ID.
			 */
			virtual void handle_disconnect( ConnectionID id );
		};

		/** Ctor.
		 * @param io_service IO service.
		 * @param handler Handler.
		 */
		Client( boost::asio::io_service& io_service, Handler& handler );

		/** Dtor.
		 */
		~Client();

		/** Check if connection started.
		 * @return true if started.
		 */
		bool is_started() const;

		/** Start.
		 * Initiates the connection.
		 * @param ip IP.
		 * @param port Port.
		 * @return true on success.
		 */
		bool start( const std::string& ip, unsigned short port );

		/** Stop.
		 * Drop connection.
		 */
		void stop();

		/** Send message to server.
		 * @param message Message.
		 */
		template <class MsgType>
		void send_message( const MsgType& message );

		/** Set handler.
		 * @param handler Handler.
		 */
		void set_handler( Handler& handler );

		/** Get handler.
		 * @return Handler.
		 */
		Handler& get_handler();

	private:
		enum { READ_BUFFER_SIZE = 1024 };

		void handle_connect( const boost::system::error_code& error );
		void start_read();
		void handle_read( const boost::system::error_code& error, std::size_t num_bytes_read );
		void handle_write( const boost::system::error_code& error, std::shared_ptr<ServerProtocol::Buffer> buffer );

		boost::asio::io_service& m_io_service;
		std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;

		char m_receive_buffer[READ_BUFFER_SIZE];
		ServerProtocol::Buffer m_buffer;

		Handler* m_handler;
		bool m_started;
};

}

#include "Client.inl"
