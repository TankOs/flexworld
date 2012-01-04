#include <FlexWorld/GameMode.hpp>

#include <cassert>

namespace flex {

GameMode::GameMode() {
}

const std::string& GameMode::get_name() const {
	return m_name;
}

const std::string& GameMode::get_author() const {
	return m_author;
}

const std::string& GameMode::get_website() const {
	return m_website;
}

const Version& GameMode::get_version() const {
	return m_version;
}

const Version& GameMode::get_min_flexworld_version() const {
	return m_min_flexworld_version;
}

const Version& GameMode::get_max_flexworld_version() const {
	return m_max_flexworld_version;
}

const FlexID& GameMode::get_thumbnail() const {
	return m_thumbnail;
}

std::size_t GameMode::get_num_packages() const {
	return m_packages.size();
}

const FlexID& GameMode::get_package( std::size_t index ) const {
	assert( index < m_packages.size() );
	return m_packages[index];
}

void GameMode::set_name( const std::string& name ) {
	m_name = name;
}

void GameMode::set_author( const std::string& author ) {
	m_author = author;
}

void GameMode::set_website( const std::string& website ) {
	m_website = website;
}

void GameMode::set_version( const Version& version ) {
	m_version = version;
}

void GameMode::set_min_flexworld_version( const Version& version ) {
	m_min_flexworld_version = version;
}

void GameMode::set_max_flexworld_version( const Version& version ) {
	m_max_flexworld_version = version;
}

void GameMode::set_thumbnail( const FlexID& thumb ) {
	assert( thumb.is_valid_resource() );
	m_thumbnail = thumb;
}

void GameMode::add_package( const FlexID& package ) {
	assert( package.is_valid_package() && !package.is_valid_resource() );
	m_packages.push_back( package );
}

}
