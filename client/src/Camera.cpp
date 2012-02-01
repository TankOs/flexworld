#include "Camera.hpp"

#include <algorithm>

Camera::Camera() :
	m_position( 0, 0, 0 ),
	m_rotation( 0, 0, 0 ),
	m_fov( 90 ),
	m_aspect( 1 ),
	m_pitch_clamp( 360.0f )
{
}

void Camera::set_fov( float fov ) {
	m_fov = fov;
}

float Camera::get_fov() const {
	return m_fov;
}

void Camera::set_aspect_ratio( float aspect ) {
	m_aspect = aspect;
}

float Camera::get_aspect_ratio() const {
	return m_aspect;
}

void Camera::set_position( const sf::Vector3f& pos ) {
	m_position = pos;
}

const sf::Vector3f& Camera::get_position() const {
	return m_position;
}

void Camera::set_rotation( const sf::Vector3f& rotation ) {
	m_rotation = rotation;
	apply_clamp();
}

const sf::Vector3f& Camera::get_rotation() const {
	return m_rotation;
}

void Camera::turn( const sf::Vector3f& angle ) {
	m_rotation += angle;
	apply_clamp();
}

void Camera::set_pitch_clamp( float clamp ) {
	m_pitch_clamp = clamp;
	apply_clamp();
}

void Camera::apply_clamp() {
	m_rotation.x = std::max( -m_pitch_clamp, std::min( m_pitch_clamp, m_rotation.x ) );
}
