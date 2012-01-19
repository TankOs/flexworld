#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>

namespace flex {

SessionHost::SessionHost(
	LockFacility& lock_facility,
	AccountManager& account_manager
) :
	m_server( *this ),
	m_lock_facility( lock_facility ),
	m_account_manager( account_manager )
{
}

const LockFacility& SessionHost::get_lock_facility() const {
	return m_lock_facility;
}

const AccountManager& SessionHost::get_account_manager() const {
	return m_account_manager;
}

void SessionHost::set_ip( const std::string& ip ) {
	m_server.set_ip( ip );
}

void SessionHost::set_port( unsigned short port ) {
	m_server.set_port( port );
}

const std::string& SessionHost::get_ip() const {
	return m_server.get_ip();
}

unsigned short SessionHost::get_port() const {
	return m_server.get_port();
}

bool SessionHost::run() {
	return m_server.run();
}

void SessionHost::stop() {
	m_server.stop();
}

bool SessionHost::is_running() const {
	return m_server.is_running();
}

void SessionHost::handle_connect( Server::ConnectionID conn_id ) {
	// Client connected, send server info.
	msg::ServerInfo msg;
	msg.set_auth_mode( msg::ServerInfo::OPEN_AUTH );
	msg.set_flags( msg::ServerInfo::NO_FLAGS );

	m_server.send_message( msg, conn_id );
}

}
