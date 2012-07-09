#include <FlexWorld/Account.hpp>

namespace fw {

Account::Account() :
	m_entity_id( 0 )
{
}

void Account::set_username( const std::string& username ) {
	m_username = username;
}

void Account::set_password( const std::string& password ) {
	m_password = password;
}

void Account::set_entity_id( Entity::ID id ) {
	m_entity_id = id;
}

const std::string& Account::get_username() const {
	return m_username;
}

const std::string& Account::get_password() const {
	return m_password;
}

Entity::ID Account::get_entity_id() const {
	return m_entity_id;
}

}
