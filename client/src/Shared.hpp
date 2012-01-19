#pragma once

#include "UserSettings.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/NonCopyable.hpp>
#include <boost/thread.hpp>
#include <memory>

/** Shared singleton.
 */
class Shared : public flex::NonCopyable {
	public:
		/** Get Shared instance.
		 * @return Shared instance.
		 */
		static Shared& get();

		UserSettings user_settings; ///< User settings.

		std::unique_ptr<flex::Client> client; ///< Client.
		std::unique_ptr<boost::thread> client_thread; ///< Client thread.

		std::unique_ptr<flex::SessionHost> host; ///< Session host.
		std::unique_ptr<boost::thread> host_thread; ///< Session host thread.

		std::unique_ptr<flex::AccountManager> account_manager; ///< Account manager.
		std::unique_ptr<flex::LockFacility> lock_facility; ///< Lock facility.

	private:
		Shared();
};

Shared& get_shared();
