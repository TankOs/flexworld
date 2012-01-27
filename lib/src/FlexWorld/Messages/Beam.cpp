#include <FlexWorld/Messages/Beam.hpp>

#include <cstring>

namespace flex {
namespace msg {

Beam::Beam() :
	Message(),
	m_position( 0, 0, 0 ),
	m_planet_size( 0, 0, 0 ),
	m_chunk_size( 0, 0, 0 ),
	m_angle( 0 )
{
}

void Beam::serialize( Buffer& buffer ) const {
	if( m_planet_name.size() == 0 || m_planet_name.size() > MAX_PLANET_NAME_LENGTH ) {
		throw InvalidDataException( "Invalid planet name." );
	}

	if( m_planet_size.x < 1 || m_planet_size.y < 1 || m_planet_size.z < 1 ) {
		throw InvalidDataException( "Invalid planet size." );
	}

	if( m_chunk_size.x < 1 || m_chunk_size.y < 1 || m_chunk_size.z < 1 ) {
		throw InvalidDataException( "Invalid chunk size." );
	}

	if( m_position.x < 0 || m_position.y < 0 || m_position.z < 0 ) {
		throw InvalidDataException( "Invalid position." );
	}

	std::size_t buf_ptr = buffer.size();

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( uint8_t ) // Planet name length.
		+ sizeof( char ) * m_planet_name.size()
		+ sizeof( m_position )
		+ sizeof( m_angle )
		+ sizeof( m_planet_size )
		+ sizeof( m_chunk_size )
	);

	buffer[buf_ptr] = static_cast<uint8_t>( m_planet_name.size() ); ++buf_ptr;
	std::memcpy( &buffer[buf_ptr], m_planet_name.c_str(), m_planet_name.size() ); buf_ptr += m_planet_name.size();

	std::memcpy( &buffer[buf_ptr], &m_position, sizeof( m_position ) ); buf_ptr += sizeof( m_position );
	std::memcpy( &buffer[buf_ptr], &m_angle, sizeof( m_angle ) ); buf_ptr += sizeof( m_angle );
	std::memcpy( &buffer[buf_ptr], &m_planet_size, sizeof( m_planet_size ) ); buf_ptr += sizeof( m_planet_size );
	std::memcpy( &buffer[buf_ptr], &m_chunk_size, sizeof( m_chunk_size ) ); buf_ptr += sizeof( m_chunk_size );
}

std::size_t Beam::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Planet name length.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t planet_name_length = buffer[buf_ptr];
	buf_ptr += sizeof( uint8_t );

	if( planet_name_length == 0 || planet_name_length > MAX_PLANET_NAME_LENGTH ) {
		throw BogusDataException( "Invalid planet name." );
	}

	// Check if username is present.
	if( buffer_size - buf_ptr < planet_name_length ) {
		return 0;
	}

	// Remember planet name ptr.
	const Buffer::value_type* planet_name_ptr = &buffer[buf_ptr];
	buf_ptr += planet_name_length;

	// Position.
	if( buffer_size - buf_ptr < sizeof( m_position ) ) {
		return 0;
	}

	sf::Vector3f position = *reinterpret_cast<const sf::Vector3f*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( position );

	if( position.x < 0 || position.y < 0 || position.z < 0 ) {
		throw BogusDataException( "Invalid position." );
	}

	// Angle.
	if( buffer_size - buf_ptr < sizeof( m_angle ) ) {
		return 0;
	}

	uint16_t angle = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( angle );

	if( angle >= 360 ) {
		throw BogusDataException( "Invalid angle." );
	}

	// Planet size.
	if( buffer_size - buf_ptr < sizeof( m_planet_size ) ) {
		return 0;
	}

	Planet::Vector planet_size = *reinterpret_cast<const Planet::Vector*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( planet_size );

	if( planet_size.x < 1 || planet_size.y < 1 || planet_size.z < 1 ) {
		throw BogusDataException( "Invalid planet size." );
	}

	// Chunk size.
	if( buffer_size - buf_ptr < sizeof( m_chunk_size ) ) {
		return 0;
	}

	Chunk::Vector chunk_size = *reinterpret_cast<const Chunk::Vector*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( chunk_size );

	if( chunk_size.x < 1 || chunk_size.y < 1 || chunk_size.z < 1 ) {
		throw BogusDataException( "Invalid chunk size." );
	}

	// Everything okay, set props.
	m_planet_name = std::string( planet_name_ptr, planet_name_length );
	m_position = position;
	m_angle = angle;
	m_planet_size = planet_size;
	m_chunk_size = chunk_size;

	return buf_ptr;
}

const std::string& Beam::get_planet_name() const {
	return m_planet_name;
}

const sf::Vector3f& Beam::get_position() const {
	return m_position;
}

uint16_t Beam::get_angle() const {
	return m_angle;
}

const Planet::Vector& Beam::get_planet_size() const {
	return m_planet_size;
}

const Chunk::Vector& Beam::get_chunk_size() const {
	return m_chunk_size;
}

void Beam::set_planet_name( const std::string& planet_name ) {
	m_planet_name = planet_name;
}

void Beam::set_position( const sf::Vector3f& position ) {
	m_position = position;
}

void Beam::set_angle( uint16_t angle ) {
	m_angle = angle % 360;
}

void Beam::set_planet_size( const Planet::Vector& planet_size ) {
	m_planet_size = planet_size;
}

void Beam::set_chunk_size( const Chunk::Vector& chunk_size ) {
	m_chunk_size = chunk_size;
}

}
}
