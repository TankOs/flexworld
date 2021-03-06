#pragma once

#include <FlexWorld/ServerProtocol.hpp>
#include <FlexWorld/MessageHandler.hpp>
#include <FlexWorld/Peer.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace fw {

/** Server for handling peers and traffic.
 *
 * Default IP is 0.0.0.0, port 2593 and 1 dispatch thread.
 * 
 * Destructing a Server object will wait until all connections are closed. Make
 * sure to always wait for run() to return so that all connections are shutdown
 * gracefully.
 */
class Server {
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
		Server( boost::asio::io_service& io_service, Handler& handler );

		/** Dtor.
		 */
		~Server();

		/** Copy ctor.
		 * @param other Other.
		 */
		Server( const Server& other ) = delete;

		/** Assignment.
		 * @param other Other.
		 */
		Server& operator=( const Server& other ) = delete;

		/** Set IP.
		 * @param ip IP.
		 */
		void set_ip( const std::string& ip );

		/** Get IP.
		 * @return IP.
		 */
		const std::string& get_ip() const;

		/** Set port.
		 * @param port Port.
		 */
		void set_port( uint16_t port );

		/** Get port.
		 * @return Port.
		 */
		uint16_t get_port() const;

		/** Get number of connected peers.
		 * @return Number of connected peers.
		 */
		std::size_t get_num_peers() const;

		/** Check if server is running.
		 * @return true if running.
		 */
		bool is_running() const;

		/** Start server.
		 * You still need to run the associated IO service so that actions are
		 * performed.
		 * @return true if everything went right.
		 */
		bool start();

		/** Stop server.
		 * Close all connections.
		 */
		void stop();

		/** Get IP of client.
		 * @param conn_id Connection ID.
		 * @return IP.
		 */
		const std::string& get_client_ip( ConnectionID conn_id ) const;

		/** Disconnect client.
		 * @param conn_id Connection ID.
		 */
		void disconnect_client( ConnectionID conn_id );

		/** Send message to single client.
		 * Exceptions by MsgType::serialize() are not catched.
		 * @param message Message.
		 * @param conn_id Client connection ID (must be valid).
		 */
		template <class MsgType>
		void send_message( const MsgType& message, ConnectionID conn_id );

	private:
		typedef std::vector<std::shared_ptr<Peer> > PeerPtrVector;

		void start_accept();
		void handle_accept( std::shared_ptr<Peer> peer, const boost::system::error_code& error );
		void start_read( std::shared_ptr<Peer> peer );
		void handle_read( std::shared_ptr<Peer> peer, const boost::system::error_code& error, std::size_t num_bytes_read );
		void handle_write( const boost::system::error_code& error, std::shared_ptr<ServerProtocol::Buffer> buffer, ConnectionID conn_id );

		PeerPtrVector m_peers;

		std::string m_ip;

		std::unique_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;

		boost::asio::io_service& m_io_service;
		Handler& m_handler;

		uint32_t m_num_peers;

		uint16_t m_port;

		bool m_running;
};

}

#include "Server.inl"
