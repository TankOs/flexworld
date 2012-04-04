#include "ExampleWorldGate.hpp"

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

