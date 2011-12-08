#include <FlexWorld/Account.hpp>

namespace flex {

Account::Account() {
}

void Account::set_username( const std::string& username ) {
	m_username = username;
}

void Account::set_password( const std::string& password ) {
	m_password = password;
}

const std::string& Account::get_username() const {
	return m_username;
}

const std::string& Account::get_password() const {
	return m_password;
}

}
