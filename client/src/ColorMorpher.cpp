#include "ColorMorpher.hpp"

#include <algorithm>
#include <cmath>

ColorMorpher::ColorMorpher() {
	reset();
}

void ColorMorpher::start( const sf::Color& from_color, const sf::Color& to_color, uint32_t duration ) {
	m_current_color = from_color;

	m_current_color_calc[0] = static_cast<float>( from_color.r );
	m_current_color_calc[1] = static_cast<float>( from_color.g );
	m_current_color_calc[2] = static_cast<float>( from_color.b );

	m_to_color[0] = static_cast<float>( to_color.r );
	m_to_color[1] = static_cast<float>( to_color.g );
	m_to_color[2] = static_cast<float>( to_color.b );

	// Calc steps.
	float f_time( static_cast<float>( duration ) );

	m_steps[0] = std::abs( m_to_color[0] - static_cast<float>( from_color.r ) ) / f_time;
	m_steps[1] = std::abs( m_to_color[1] - static_cast<float>( from_color.g ) ) / f_time;
	m_steps[2] = std::abs( m_to_color[2] - static_cast<float>( from_color.b ) ) / f_time;
}

void ColorMorpher::update( uint32_t delta_t ) {
	if( m_steps[0] == 0 && m_steps[1] == 0 && m_steps[2] == 0 ) {
		return;
	}

	float f_time( static_cast<float>( delta_t ) );

	for( std::size_t index = 0; index < 3; ++index ) {
		if( m_steps[index] == 0.f ) {
			continue;
		}

		if( m_current_color_calc[index] < m_to_color[index] ) {
			m_current_color_calc[index] = std::min( m_to_color[index], m_current_color_calc[index] + m_steps[index] * f_time );
		}
		else if( m_current_color_calc[index] > m_to_color[index] ) {
			m_current_color_calc[index] = std::max( m_to_color[index], m_current_color_calc[index] - m_steps[index] * f_time );
		}

		if( m_current_color_calc[index] == m_to_color[index] ) {
			m_steps[index] = 0.f;
		}
	}

	m_current_color.r = static_cast<uint8_t>( m_current_color_calc[0] );
	m_current_color.g = static_cast<uint8_t>( m_current_color_calc[1] );
	m_current_color.b = static_cast<uint8_t>( m_current_color_calc[2] );
}

const sf::Color& ColorMorpher::get_current_color() const {
	return m_current_color;
}

void ColorMorpher::reset() {
	m_steps[0] = 0;
	m_steps[1] = 0;
	m_steps[2] = 0;
}
