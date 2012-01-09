#pragma once

#include <FlexWorld/Socket.hpp>
#include <FlexWorld/NonCopyable.hpp>

#include <boost/thread/mutex.hpp>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace boost {
class thread;
}

namespace flex {

class Protocol;
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
		/** Ctor.
		 * @param protocol Protocol.
		 */
		Server( Protocol& protocol );

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
		typedef std::vector<Peer*> PeerPtrVector;
		typedef std::vector<boost::thread*> ThreadPtrVector;

		void clean_threads();
		void clean_sockets();
		void wait_for_threads();
		void run_dispatcher();

		std::unique_ptr<Socket> m_listener;
		PeerPtrVector m_peers;
		ThreadPtrVector m_dispatch_threads;
		std::string m_ip;
		Protocol& m_protocol;
		std::size_t m_num_dispatch_threads;
		uint32_t m_num_peers;
		uint16_t m_port;
		bool m_running;

		boost::mutex m_work_avail_mutex;
};

}
