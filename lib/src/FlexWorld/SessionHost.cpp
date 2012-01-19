#include <FlexWorld/SessionHost.hpp>

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

}
