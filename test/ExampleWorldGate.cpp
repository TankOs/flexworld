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

void ExampleWorldGate::set_block( const BlockPosition& block_position, const std::string& planet, const fw::FlexID& cls ) {
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

uint32_t ExampleWorldGate::create_entity( const fw::FlexID& cls_id, const EntityPosition& position, const std::string& planet_id ) {
	if( cls_id.get() != "some/class" ) {
		throw std::runtime_error( "Invalid class." );
	}

	if( position != EntityPosition( 11, 22, 33 ) ) {
		throw std::runtime_error( "Invalid entity position." );
	}

	if( planet_id != "planet" ) {
		throw std::runtime_error( "Invalid planet." );
	}

	return 938;
}

uint32_t ExampleWorldGate::create_entity( const fw::FlexID& cls_id, uint32_t parent_id, const std::string& hook_id ) {
	if( cls_id.get() != "some/class348734" ) {
		throw std::runtime_error( "Invalid class." );
	}

	if( parent_id != 9384 ) {
		throw std::runtime_error( "Invalid parent entity ID." );
	}

	if( hook_id != "foobar387" ) {
		throw std::runtime_error( "Invalid hook." );
	}

	return 458;
}

uint32_t ExampleWorldGate::create_entity( const fw::FlexID& cls_id, uint32_t container_id ) {
	if( cls_id.get() != "some/class12347882" ) {
		throw std::runtime_error( "Invalid class." );
	}

	if( container_id != 3478834 ) {
		throw std::runtime_error( "Invalid container ID." );
	}

	return 784;
}

void ExampleWorldGate::get_entity_position( uint32_t entity_id, EntityPosition& position, std::string& planet_id ) {
	if( entity_id != 0 ) {
		throw std::runtime_error( "Invalid entity ID." );
	}

	position = EntityPosition( 1, 2, 3 );
	planet_id = "planet";
}

std::string ExampleWorldGate::get_entity_class_id( uint32_t entity_id ) {
	if( entity_id != 0 ) {
		throw std::runtime_error( "Invalid entity ID." );
	}

	return "hax/class";
}
