#include <FlexWorld/Messages/CreateEntity.hpp>

#include <cstring>

namespace flex {
namespace msg {

CreateEntity::CreateEntity() :
	Message(),
	m_class( "" ),
	m_parent_hook( "" ),
	m_position( 0, 0, 0 ),
	m_id( 0 ),
	m_parent_id( 0 ),
	m_heading( 0 )
{
}

void CreateEntity::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	if( m_class.size() < 1 || m_class.size() > 255 ) {
		throw InvalidDataException( "Invalid class name." );
	}
	else if( m_parent_hook.size() > 255 ) {
		throw InvalidDataException( "Invalid hook." );
	}

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( m_id )
		+ sizeof( m_position )
		+ sizeof( m_heading )
		+ sizeof( uint8_t ) // Class length.
	);

	*reinterpret_cast<Entity::ID*>( &buffer[buf_ptr] ) = m_id; buf_ptr += sizeof( m_id );
	*reinterpret_cast<Planet::Coordinate*>( &buffer[buf_ptr] ) = m_position; buf_ptr += sizeof( m_position );
	*reinterpret_cast<float*>( &buffer[buf_ptr] ) = m_heading; buf_ptr += sizeof( m_heading );
	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_class.size() ); buf_ptr += sizeof( uint8_t );
	buffer.insert( buffer.end(), m_class.begin(), m_class.end() );

	// Insert hook length, hook and parent ID (last two only if desired).
	uint8_t hook_length = static_cast<uint8_t>( m_parent_hook.size() );

	buffer.insert( buffer.end(), reinterpret_cast<const char*>( &hook_length ), reinterpret_cast<const char*>( &hook_length ) + sizeof( hook_length ) );

	// Append hook name and parent ID if hook is set.
	if( hook_length > 0 ) {
		buffer.insert( buffer.end(), m_parent_hook.begin(), m_parent_hook.end() );
		buffer.insert( buffer.end(), reinterpret_cast<const char*>( &m_parent_id ), reinterpret_cast<const char*>( &m_parent_id ) + sizeof( m_parent_id ) );
	}
}

std::size_t CreateEntity::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// ID.
	if( buffer_size - buf_ptr < sizeof( m_id ) ) {
		return 0;
	}

	Entity::ID id = *reinterpret_cast<const Entity::ID*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( id );

	// Position.
	if( buffer_size - buf_ptr < sizeof( m_position ) ) {
		return 0;
	}

	Planet::Coordinate position = *reinterpret_cast<const Planet::Coordinate*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( position );

	// Heading.
	if( buffer_size - buf_ptr < sizeof( m_heading ) ) {
		return 0;
	}

	float heading = *reinterpret_cast<const float*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( heading );

	// Class length.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t class_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( class_length );

	if( class_length < 1 ) {
		throw BogusDataException( "Invalid class length." );
	}

	// Class.
	if( buffer_size - buf_ptr < class_length ) {
		return 0;
	}

	const char* class_ptr = &buffer[buf_ptr];
	buf_ptr += class_length;

	// Hook length.
	uint8_t hook_length = 0;

	if( buffer_size - buf_ptr < sizeof( hook_length ) ) {
		return 0;
	}

	hook_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( hook_length );

	// Read hook and parent entity ID if hook length is given.
	const char* hook_id = nullptr;
	Entity::ID parent_id = 0;

	if( hook_length > 0 ) {
		// Hook.
		if( buffer_size - buf_ptr < hook_length ) {
			return 0;
		}

		hook_id = &buffer[buf_ptr];
		buf_ptr += hook_length;

		// Parent entity ID.
		if( buffer_size - buf_ptr < sizeof( m_parent_id ) ) {
			return 0;
		}

		parent_id = *reinterpret_cast<const Entity::ID*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( m_parent_id );
	}

	// Okay, apply.
	m_id = id;
	m_position = position;
	m_heading = heading;
	m_class = std::string( class_ptr, class_length );

	if( hook_length == 0 ) {
		m_parent_hook = "";
		m_parent_id = 0;
	}
	else {
		m_parent_hook = std::string( hook_id, hook_length );
		m_parent_id = parent_id;
	}

	return buf_ptr;
}

void CreateEntity::set_position( const Planet::Coordinate& pos ) {
	m_position = pos;
}

const Planet::Coordinate& CreateEntity::get_position() const {
	return m_position;
}

void CreateEntity::set_heading( float heading ) {
	m_heading = heading;
}

float CreateEntity::get_heading() const {
	return m_heading;
}

void CreateEntity::set_class( const std::string& cls ) {
	m_class = cls;
}

const std::string& CreateEntity::get_class() const {
	return m_class;
}

void CreateEntity::set_id( Entity::ID id ) {
	m_id = id;
}

Entity::ID CreateEntity::get_id() const {
	return m_id;
}

bool CreateEntity::has_parent() const {
	return !m_parent_hook.empty();
}

Entity::ID CreateEntity::get_parent_id() const {
	return m_parent_id;
}

const std::string& CreateEntity::get_parent_hook() const {
	return m_parent_hook;
}

void CreateEntity::set_parent_id( Entity::ID id ) {
	m_parent_id = id;
}

void CreateEntity::set_parent_hook( const std::string& hook ) {
	m_parent_hook = hook;
}

}
}
