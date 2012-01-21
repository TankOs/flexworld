#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Log.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/Account.hpp>
#include <FlexWorld/World.hpp>

namespace flex {

SessionHost::SessionHost(
	LockFacility& lock_facility,
	AccountManager& account_manager,
	World& world
) :
	m_auth_mode( OPEN_AUTH ),
	m_lock_facility( lock_facility ),
	m_account_manager( account_manager ),
	m_world( world )
{
	m_server.reset( new Server( *this ) );
}

const LockFacility& SessionHost::get_lock_facility() const {
	return m_lock_facility;
}

const AccountManager& SessionHost::get_account_manager() const {
	return m_account_manager;
}

void SessionHost::set_ip( const std::string& ip ) {
	m_server->set_ip( ip );
}

void SessionHost::set_port( unsigned short port ) {
	m_server->set_port( port );
}

const std::string& SessionHost::get_ip() const {
	return m_server->get_ip();
}

unsigned short SessionHost::get_port() const {
	return m_server->get_port();
}

bool SessionHost::run() {
	// Make sure fw.base.nature/grass is present for construction the planet
	// "construct".
	FlexID id;
	id.parse( "fw.base.nature/grass" );

	const Class* grass_cls = m_world.find_class( id );
	if( !grass_cls ) {
		Log::Logger( Log::FATAL ) << id.get() << " doesn't exist but needed for planet \"construct\"." << Log::endl;
		return false;
	}

	return m_server->run();
}

void SessionHost::stop() {
	m_server->stop();
}

bool SessionHost::is_running() const {
	return m_server->is_running();
}

void SessionHost::handle_connect( Server::ConnectionID conn_id ) {
	Log::Logger( Log::INFO ) << "Client connected from " << m_server->get_client_ip( conn_id ) << "." << Log::endl;

	// Client connected, send server info.
	msg::ServerInfo msg;
	msg.set_auth_mode( m_auth_mode == OPEN_AUTH ? msg::ServerInfo::OPEN_AUTH : msg::ServerInfo::KEY_AUTH );
	msg.set_flags( msg::ServerInfo::NO_FLAGS );

	m_server->send_message( msg, conn_id );
}

void SessionHost::handle_message( const msg::OpenLogin& login_msg, Server::ConnectionID conn_id ) {
	const std::string& remote_ip = m_server->get_client_ip( conn_id );

	// If we're not in open auth mode, disconnect client.
	if( m_auth_mode != OPEN_AUTH ) {
		Log::Logger( Log::INFO ) << "Client " << remote_ip << " tried to open-auth, but we're setup for key auth." << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

	bool is_local = false;

	// Check if client is a local client.
	if( m_server->get_client_ip( conn_id ) == "127.0.0.1" ) {
		is_local = true;
	}

	if( !is_local ) {
		Log::Logger( Log::INFO ) << "Client wants to login as " << login_msg.get_username() << "." << Log::endl;
	}
	else {
		Log::Logger( Log::INFO ) << "Client " << login_msg.get_username() << " is connecting from this machine, authenticated." << Log::endl;
	}

	m_lock_facility.lock_account_manager( true );

	// Check if an account for that username exists.
	const flex::Account* account = m_account_manager.find_account( login_msg.get_username() );

	// If it doesn't exist, create a new one. TODO: Make this configurable?
	if( account == nullptr ) {
		flex::Account new_account;
		new_account.set_username( login_msg.get_username() );
		new_account.set_password( login_msg.get_password() );
		m_account_manager.add_account( new_account );

		account = m_account_manager.find_account( login_msg.get_username() );
		assert( account != nullptr );
	}
	else {
		// Check for correct password.
		if( account->get_password() != login_msg.get_password() ) {
			Log::Logger( Log::INFO ) << "Client " << remote_ip << " gave wrong password for account " << login_msg.get_password() << "." << Log::endl;
			m_server->disconnect_client( conn_id );
			return;
		}
	}

	m_lock_facility.lock_account_manager( false );

	// Everything is good, log the player in!
	msg::LoginOK ok_msg;
	m_server->send_message( ok_msg, conn_id );
}

void SessionHost::set_auth_mode( AuthMode mode ) {
	m_auth_mode = mode;
}

SessionHost::AuthMode SessionHost::get_auth_mode() const {
	return m_auth_mode;
}

const World& SessionHost::get_world() const {
	return m_world;
}

}
