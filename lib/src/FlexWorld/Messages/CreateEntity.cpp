#include <FlexWorld/Messages/CreateEntity.hpp>

#include <cstring>

namespace flex {
namespace msg {

CreateEntity::CreateEntity() :
	Message(),
	m_position( 0, 0, 0 ),
	m_heading( 0 )
{
}

void CreateEntity::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	if( m_class.size() < 1 || m_class.size() > 255 ) {
		throw InvalidDataException( "Invalid class name: " + m_class );
	}

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( m_position )
		+ sizeof( m_heading )
		+ sizeof( uint8_t )
	);

	*reinterpret_cast<Planet::Coordinate*>( &buffer[buf_ptr] ) = m_position; buf_ptr += sizeof( m_position );
	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = m_heading; buf_ptr += sizeof( m_heading );
	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_class.size() ); buf_ptr += sizeof( uint8_t );
	buffer.insert( buffer.end(), m_class.begin(), m_class.end() );
}

std::size_t CreateEntity::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

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

	uint8_t heading = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( heading );

	// Class length.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t class_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( class_length );

	// Class.
	if( buffer_size - buf_ptr < class_length ) {
		return 0;
	}

	const char* class_ptr = &buffer[buf_ptr];
	buf_ptr += class_length;

	// Okay, apply.
	m_position = position;
	m_heading = heading;
	m_class = std::string( class_ptr, class_length );

	return buf_ptr;
}

void CreateEntity::set_position( const Planet::Coordinate& pos ) {
	m_position = pos;
}

const Planet::Coordinate& CreateEntity::get_position() const {
	return m_position;
}

void CreateEntity::set_heading( uint8_t heading ) {
	m_heading = heading;
}

uint8_t CreateEntity::get_heading() const {
	return m_heading;
}

void CreateEntity::set_class( const std::string& cls ) {
	m_class = cls;
}

const std::string& CreateEntity::get_class() const {
	return m_class;
}

}
}
