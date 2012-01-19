#pragma once

#include <FlexWorld/Server.hpp>

namespace flex {

class LockFacility;
class AccountManager;

/** SessionHost.
 *
 * Represents a gaming session and implements logics, server-side protocol handlers, scripting engine etc.
 */
class SessionHost : private Server::Handler {
	public:
		/** Ctor.
		 * References to the objects are stored, therefore they must be kept alive!
		 * @param lock_facility Lock facility.
		 * @param account_manager Account manager.
		 */
		SessionHost(
			LockFacility& lock_facility,
			AccountManager& account_manager
		);

		/** Get lock facility.
		 * @return Lock facility.
		 */
		const LockFacility& get_lock_facility() const;

		/** Get account manager.
		 * @return Account manager.
		 */
		const AccountManager& get_account_manager() const;

		/** Set server IP.
		 * @param ip IP.
		 */
		void set_ip( const std::string& ip );

		/** Set server port.
		 * @param port Port.
		 */
		void set_port( unsigned short port );

		/** Get server IP.
		 * @return IP.
		 */
		const std::string& get_ip() const;

		/** Get server port.
		 * @return Port.
		 */
		unsigned short get_port() const;

		/** Run!
		 * @return true on success.
		 */
		bool run();

		/** Stop host.
		 * Make sure to wait for run() to return.
		 */
		void stop();

		/** Check if host is running.
		 * @return true if running.
		 */
		bool is_running() const;

	private:
		Server m_server;

		LockFacility& m_lock_facility;
		AccountManager& m_account_manager;
};

}
