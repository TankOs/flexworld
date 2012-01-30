#pragma once

#include <FlexWorld/Server.hpp>
#include <FlexWorld/PlayerInfo.hpp>

#include <memory>

namespace flex {

class LockFacility;
class AccountManager;
class World;

/** SessionHost.
 *
 * Represents a gaming session and implements logics, server-side protocol handlers, scripting engine etc.
 */
class SessionHost : private Server::Handler {
	public:
		/** Auth mode.
		 */
		enum AuthMode {
			OPEN_AUTH = 0,
			KEY_AUTH
		};

		/** Ctor.
		 * References to the objects are stored, therefore they must be kept alive!
		 * @param lock_facility Lock facility.
		 * @param account_manager Account manager.
		 * @param world World.
		 */
		SessionHost(
			LockFacility& lock_facility,
			AccountManager& account_manager,
			World& world
		);

		/** Get lock facility.
		 * @return Lock facility.
		 */
		const LockFacility& get_lock_facility() const;

		/** Get account manager.
		 * @return Account manager.
		 */
		const AccountManager& get_account_manager() const;

		/** Get world.
		 * @return World.
		 */
		const World& get_world() const;

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

		/** Set auth mode.
		 * @param mode Auth mode.
		 */
		void set_auth_mode( AuthMode mode );

		/** Get auth mode.
		 * @return Auth mode.
		 */
		AuthMode get_auth_mode() const;

		/** Set player limit.
		 * @param limit Player limit.
		 */
		void set_player_limit( std::size_t limit );

		/** Get player limit.
		 * @return Player limit.
		 */
		std::size_t get_player_limit() const;

	private:
		typedef std::vector<PlayerInfo> PlayerInfoVector;

		void handle_connect( Server::ConnectionID conn_id );
		void handle_disconnect( Server::ConnectionID conn_id );
		void handle_message( const msg::OpenLogin& login_msg, Server::ConnectionID conn_id );
		void handle_message( const msg::Ready& login_msg, Server::ConnectionID conn_id );
		void handle_message( const msg::RequestChunk& req_chunk_msg, Server::ConnectionID conn_id );

		void beam_player( Server::ConnectionID conn_id, const std::string& planet_id, const sf::Vector3f& position, uint16_t angle );

		PlayerInfoVector m_player_infos;

		LockFacility& m_lock_facility;
		AccountManager& m_account_manager;
		World& m_world;

		std::unique_ptr<Server> m_server;

		AuthMode m_auth_mode;
		std::size_t m_player_limit;
		Planet::ScalarType m_max_view_radius;
};

}
