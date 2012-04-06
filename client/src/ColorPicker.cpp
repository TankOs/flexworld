#include "ColorPicker.hpp"

ColorPicker::Result ColorPicker::pick(
	const sf::Vector3f& origin_v,
	const sf::Vector3f fwd_v,
	float distance,
	const flex::Planet& planet
) {
	Result result;
	result.m_type = Result::NONE;

	return result;
}

ColorPicker::Result::Result() :
	m_type( NONE ),
	m_block_position( 0, 0, 0 ),
	m_entity_id( 0 )
{
}
