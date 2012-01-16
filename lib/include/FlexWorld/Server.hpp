#pragma once

#include <FlexWorld/Socket.hpp>
#include <FlexWorld/Selector.hpp>
#include <FlexWorld/NonCopyable.hpp>
#include <FlexWorld/ServerProtocol.hpp>
#include <FlexWorld/MessageHandler.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace boost {
class thread;
}

namespace flex {

class Peer;


/** Server for handling peers and traffic.
 * Configure the server before calling run(). The run() method will launch N
 * dispatch threads as configured but still blocks for accepting incoming
 * connections and receiving data (in the caller's thread).
 *
 * Default IP is 0.0.0.0, port 2593 and 1 dispatch thread.
 * 
 * Destructing a Server object will wait until all dispatchers are finished
 * with their work.
 *
 * Make sure to always wait for run() to return so that all connections are
 * shutdown gracefully. Otherwise lingering connections will stay open in
 * TIME_WAIT state for ~4 minutes.
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
		 * Make sure to shut the server down gracefully with shutdown() before.
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

		/** Shutdown server (thread-safe).
		 * This will gracefully disconnect all peers and shutdown the listener.
		 * Make sure to still wait for run() to return.
		 */
		void shutdown();

		/** Set number of dispatch threads.
		 * @param num_threads Number of threads.
		 */
		void set_num_dispatch_threads( std::size_t num_threads );

		/** Get number of dispatch threads.
		 * @return Number of threads.
		 */
		std::size_t get_num_dispatch_threads() const;

		/** Run (thread-safe).
		 * @return true if everything went right.
		 */
		bool run();

	private:
		typedef std::vector<std::shared_ptr<Peer>> PeerPtrVector;
		typedef std::map<Socket*, ConnectionID> SocketIDMap;
		typedef std::vector<boost::thread*> ThreadPtrVector;

		void cleanup(); // Wait for threads, clean threads, clean sockets, clean socket->ID mappings.
		void notify_workers();
		void run_dispatcher();

		void process_listener();
		void process_peers();

		std::unique_ptr<Socket> m_listener;
		PeerPtrVector m_peers;
		ThreadPtrVector m_dispatch_threads;

		std::string m_ip;
		Selector m_selector;

		std::size_t m_num_dispatch_threads;
		Handler& m_handler;

		uint32_t m_num_peers;

		uint16_t m_port;

		bool m_running;

		boost::condition m_work_condition;
		boost::mutex m_worker_mutex;
		boost::mutex m_work_data_mutex;
};

}
