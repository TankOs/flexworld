#include <FlexWorld/LuaModules/World.hpp>
#include <FlexWorld/LuaModules/WorldGate.hpp>
#include <FlexWorld/FlexID.hpp>

#include <Diluculum/LuaExceptions.hpp>
#include <Diluculum/LuaWrappers.hpp>

#include <cstdint>

namespace flex {
namespace lua {

DILUCULUM_BEGIN_CLASS( World )
	DILUCULUM_CLASS_METHOD( World, create_entity )
	DILUCULUM_CLASS_METHOD( World, destroy_block )
	DILUCULUM_CLASS_METHOD( World, get_entity_position )
	DILUCULUM_CLASS_METHOD( World, set_block )
DILUCULUM_END_CLASS( World )

void World::register_class( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_CLASS( target, World );
}

World::World( const Diluculum::LuaValueList& /*args*/ ) :
	m_gate( nullptr )
{
	throw Diluculum::LuaError( "Not allowed to instantiate World." );
}

World::World( WorldGate& gate ) :
	m_gate( &gate )
{
}

void World::register_object( Diluculum::LuaVariable target ) {
	DILUCULUM_REGISTER_OBJECT( target, World, *this );
}

Diluculum::LuaValueList World::destroy_block( const Diluculum::LuaValueList& args ) {
	if( args.size() != 2 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TTABLE ) {
		throw Diluculum::LuaError( "Expected table for position." );
	}
	else if( args[1].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for planet." );
	}

	Diluculum::LuaValueMap position_table = args[0].asTable();

	if( position_table.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of elements in position table." );
	}
	else if( position_table[1].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for x position." );
	}
	else if( position_table[2].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for y position." );
	}
	else if( position_table[3].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for z position." );
	}

	uint32_t x = static_cast<uint32_t>( position_table[1].asNumber() );
	uint32_t y = static_cast<uint32_t>( position_table[2].asNumber() );
	uint32_t z = static_cast<uint32_t>( position_table[3].asNumber() );
	std::string planet = args[1].asString();

	if( planet.empty() ) {
		throw Diluculum::LuaError( "Invalid planet." );
	}

	// Planet may only contain alpha-numeric characters.
	for( std::size_t ch_idx = 0; ch_idx < planet.size(); ++ch_idx ) {
		char ch = planet[ch_idx];

		if(
			(ch < 'a' || ch > 'z') &&
			(ch < 'A' || ch > 'Z') &&
			(ch < '0' || ch > '9')
		) {
			throw Diluculum::LuaError( "Invalid planet." );
		}
	}

	try {
		m_gate->destroy_block( WorldGate::BlockPosition( x, y, z ), planet );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList World::set_block( const Diluculum::LuaValueList& args ) {
	if( args.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TTABLE ) {
		throw Diluculum::LuaError( "Expected table for position." );
	}
	else if( args[1].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for planet." );
	}
	else if( args[2].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for class." );
	}

	Diluculum::LuaValueMap position_table = args[0].asTable();

	if( position_table.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of elements in position table." );
	}
	else if( position_table[1].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for x position." );
	}
	else if( position_table[2].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for y position." );
	}
	else if( position_table[3].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for z position." );
	}

	uint32_t x = static_cast<uint32_t>( position_table[1].asNumber() );
	uint32_t y = static_cast<uint32_t>( position_table[2].asNumber() );
	uint32_t z = static_cast<uint32_t>( position_table[3].asNumber() );

	std::string planet = args[1].asString();
	std::string cls = args[2].asString();

	if( planet.empty() ) {
		throw Diluculum::LuaError( "Invalid planet." );
	}

	// Check valid class.
	FlexID cls_id;

	if( !cls_id.parse( cls ) || !cls_id.is_valid_resource() ) {
		throw Diluculum::LuaError( "Invalid class." );
	}

	try {
		m_gate->set_block( WorldGate::BlockPosition( x, y, z ), planet, cls_id );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList World::create_entity( const Diluculum::LuaValueList& args ) {
	if( args.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for class." );
	}
	else if( args[1].type() != LUA_TTABLE ) {
		throw Diluculum::LuaError( "Expected table for position." );
	}
	else if( args[2].type() != LUA_TSTRING ) {
		throw Diluculum::LuaError( "Expected string for planet." );
	}

	// Get position.
	Diluculum::LuaValueMap position_table = args[1].asTable();

	if( position_table.size() != 3 ) {
		throw Diluculum::LuaError( "Wrong number of elements in position table." );
	}
	else if( position_table[1].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for x position." );
	}
	else if( position_table[2].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for y position." );
	}
	else if( position_table[3].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for z position." );
	}

	std::string cls = args[0].asString();

	WorldGate::EntityPosition position(
		static_cast<float>( position_table[1].asNumber() ),
		static_cast<float>( position_table[2].asNumber() ),
		static_cast<float>( position_table[3].asNumber() )
	);

	std::string planet = args[2].asString();

	if( position.x < 0.0f ) {
		throw Diluculum::LuaError( "Invalid x position." );
	}
	else if( position.y < 0.0f ) {
		throw Diluculum::LuaError( "Invalid y position." );
	}
	else if( position.z < 0.0f ) {
		throw Diluculum::LuaError( "Invalid z position." );
	}

	if( planet.empty() ) {
		throw Diluculum::LuaError( "Invalid planet." );
	}

	// Check class ID.
	FlexID cls_id;

	if( !cls_id.parse( cls ) ) {
		throw Diluculum::LuaError( "Invalid class." );
	}
	else if( !cls_id.is_valid_resource() ) {
		throw Diluculum::LuaError( "Invalid class." );
	}

	try {
		m_gate->create_entity( cls_id, position, planet );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return Diluculum::LuaValueList();
}

Diluculum::LuaValueList World::get_entity_position( const Diluculum::LuaValueList& args ) {
	// Check arguments.
	if( args.size() != 1 ) {
		throw Diluculum::LuaError( "Wrong number of arguments." );
	}

	if( args[0].type() != LUA_TNUMBER ) {
		throw Diluculum::LuaError( "Expected number for entity." );
	}

	uint32_t entity_id = static_cast<uint32_t>( args[0].asNumber() );
	Diluculum::LuaValueList ret;

	try {
		WorldGate::EntityPosition position;
		std::string planet_id;

		m_gate->get_entity_position( entity_id, position, planet_id );

		Diluculum::LuaValueMap l_pos;
		l_pos[1] = position.x;
		l_pos[2] = position.y;
		l_pos[3] = position.z;

		ret.push_back( l_pos );
		ret.push_back( planet_id );
	}
	catch( const std::runtime_error& e ) {
		throw Diluculum::LuaError( e.what() );
	}

	return ret;
}

}
}
