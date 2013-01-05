#pragma once

#include "UserSettings.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/World.hpp>
#include <FlexWorld/Entity.hpp>
#include <memory>

namespace boost {
namespace asio {

class io_service;

}
}

/** Shared singleton.
 */
class Shared {
	public:
		/** Get Shared instance.
		 * @return Shared instance.
		 */
		static Shared& get();

		/** Copy ctor.
		 * @param other Other.
		 */
		Shared( const Shared& other ) = delete;

		/** Assignment.
		 * @param other Other.
		 */
		Shared& operator=( const Shared& other ) = delete;

		UserSettings user_settings; ///< User settings.

		std::unique_ptr<boost::asio::io_service> io_service; ///< IO service.
		std::unique_ptr<fw::Client> client; ///< Client.
		std::unique_ptr<fw::SessionHost> host; ///< Session host.

		std::unique_ptr<fw::AccountManager> account_manager; ///< Account manager.
		std::unique_ptr<fw::LockFacility> lock_facility; ///< Lock facility.
		std::unique_ptr<fw::World> world; ///< World.

		fw::Entity::ID entity_id; ///< The player's own entity ID.

	private:
		Shared();
};

Shared& get_shared();
