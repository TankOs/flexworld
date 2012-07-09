#include <FlexWorld/Messages/AttachEntity.hpp>

#include <cstring>
#include <cassert>

namespace fw {
namespace msg {

AttachEntity::AttachEntity() :
	m_source_entity_id( 0 ),
	m_target_entity_id( 0 ),
	m_hook_id( "" )
{
}

void AttachEntity::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	if( m_hook_id.empty() || m_hook_id.size() > 255 ) {
		throw InvalidDataException( "Invalid hook ID." );
	}

	buffer.resize(
		+ buf_ptr
		+ sizeof( m_source_entity_id )
		+ sizeof( m_target_entity_id )
		+ sizeof( uint8_t )
		+ m_hook_id.size()
	);

	*reinterpret_cast<uint32_t*>( &buffer[buf_ptr] ) = m_source_entity_id;
	buf_ptr += sizeof( m_source_entity_id );

	*reinterpret_cast<uint32_t*>( &buffer[buf_ptr] ) = m_target_entity_id;
	buf_ptr += sizeof( m_target_entity_id );

	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_hook_id.size() );
	buf_ptr += sizeof( uint8_t );

	std::memcpy( &buffer[buf_ptr], m_hook_id.c_str(), m_hook_id.size() );
}

std::size_t AttachEntity::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Source entity ID.
	if( buffer_size - buf_ptr < sizeof( m_source_entity_id ) ) {
		return 0;
	}

	uint32_t source_entity_id = *reinterpret_cast<const uint32_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( source_entity_id );

	// Target entity ID.
	if( buffer_size - buf_ptr < sizeof( m_target_entity_id ) ) {
		return 0;
	}

	uint32_t target_entity_id = *reinterpret_cast<const uint32_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( target_entity_id );

	// Hook ID length.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t hook_id_size = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( uint8_t );

	if( hook_id_size == 0 ) {
		throw BogusDataException( "Invalid hook ID length." );
	}

	// Hook ID.
	if( buffer_size - buf_ptr < hook_id_size ) {
		return 0;
	}

	const char* hook_id_ptr = &buffer[buf_ptr];
	buf_ptr += hook_id_size;

	// Okay, apply.
	m_source_entity_id = source_entity_id;
	m_target_entity_id = target_entity_id;
	m_hook_id = std::string( hook_id_ptr, hook_id_size );

	return buf_ptr;
}

uint32_t AttachEntity::get_source_entity_id() const {
	return m_source_entity_id;
}

uint32_t AttachEntity::get_target_entity_id() const {
	return m_target_entity_id;
}

const std::string& AttachEntity::get_hook_id() const {
	return m_hook_id;
}

void AttachEntity::set_source_entity_id( uint32_t id ) {
	m_source_entity_id = id;
}

void AttachEntity::set_target_entity_id( uint32_t id ) {
	m_target_entity_id = id;
}

void AttachEntity::set_hook_id( const std::string& id ) {
	assert( id.empty() == false );

	m_hook_id = id;
}

}
}
