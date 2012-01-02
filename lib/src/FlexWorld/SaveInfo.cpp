#include <FlexWorld/SaveInfo.hpp>

namespace flex {

SaveInfo::SaveInfo() :
	m_timestamp( 0 )
{
}

void SaveInfo::set_name( const std::string& name ) {
	m_name = name;
}

void SaveInfo::set_entities_path( const std::string& entities_path ) {
	m_entities_path = entities_path;
}

void SaveInfo::set_planets_path( const std::string& planets_path ) {
	m_planets_path = planets_path;
}

void SaveInfo::set_accounts_path( const std::string& accounts_path ) {
	m_accounts_path = accounts_path;
}

void SaveInfo::set_timestamp( uint32_t timestamp ) {
	m_timestamp = timestamp;
}

const std::string& SaveInfo::get_name() const {
	return m_name;
}

const std::string& SaveInfo::get_entities_path() const {
	return m_entities_path;
}

const std::string& SaveInfo::get_planets_path() const {
	return m_planets_path;
}

const std::string& SaveInfo::get_accounts_path() const {
	return m_accounts_path;
}

uint32_t SaveInfo::get_timestamp() const {
	return m_timestamp;
}

}
