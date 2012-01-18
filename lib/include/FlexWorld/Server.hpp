#pragma once

#include <FlexWorld/NonCopyable.hpp>
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

namespace flex {

/** Server for handling peers and traffic.
 *
 * Default IP is 0.0.0.0, port 2593 and 1 dispatch thread.
 * 
 * Destructing a Server object will wait until all connections are closed. Make
 * sure to always wait for run() to return so that all connections are shutdown
 * gracefully.
 */
class Server : public NonCopyable {
	public:
		typedef ServerProtocol::ConnectionID ConnectionID; ///< Connection ID.

		/** Handler interface.
		 */
		struct Handler : public MessageHandler<ServerMessageList, ConnectionID> {
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
		 * @param handler Handler.
		 */
		Server( Handler& handler );

		/** Dtor.
		 * Make sure to shut the server down gracefully with stop() before.
		 * Read the notes there!
		 */
		~Server();

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

		/** Get number of connected peers (thread-safe).
		 * @return Number of connected peers.
		 */
		std::size_t get_num_peers() const;

		/** Check if server is running (thread-safe).
		 * @return true if running.
		 */
		bool is_running() const;

		/** Stop server.
		 * This will gracefully disconnect all peers and shutdown the listener.
		 * Make sure to still wait for run() to return.
		 */
		void stop();

		/** Run (thread-safe).
		 * @return true if everything went right.
		 */
		bool run();

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
		Handler& m_handler;

		uint32_t m_num_peers;

		uint16_t m_port;

		bool m_running;
};

}

#include "Server.inl"
