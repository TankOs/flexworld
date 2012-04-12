#include "ExampleWorldGate.hpp"

#include <FlexWorld/FlexID.hpp>

#include <stdexcept>

void ExampleWorldGate::destroy_block( const BlockPosition& block_position, const std::string& planet ) {
	if(
		block_position.x != 1 ||
		block_position.y != 2 ||
		block_position.z != 3
	) {
		throw std::runtime_error( "Invalid block position." );
	}

	if( planet != "foobar" ) {
		throw std::runtime_error( "Invalid planet." );
	}
}

void ExampleWorldGate::set_block( const BlockPosition& block_position, const std::string& planet, const flex::FlexID& cls ) {
	if(
		block_position.x != 10 ||
		block_position.y != 20 ||
		block_position.z != 30
	) {
		throw std::runtime_error( "Invalid block position." );
	}

	if( cls.get() != "some/class" ) {
		throw std::runtime_error( "Invalid class." );
	}

	if( planet != "planet" ) {
		throw std::runtime_error( "Invalid planet." );
	}
}
