#include <FlexWorld/Messages/Chat.hpp>

#include <cassert>

namespace flex {
namespace msg {

Chat::Chat() :
	Message()
{
}

void Chat::serialize( Buffer& buffer ) const {
	// Check values.
	if( m_message.empty() || m_message.size() > 0xffff ) {
		throw InvalidDataException( "Invalid message." );
	}

	if( m_sender.empty() || m_sender.size() > 0xff ) {
		throw InvalidDataException( "Invalid sender." );
	}

	if( m_target.empty() || m_target.size() > 0xff ) {
		throw InvalidDataException( "Invalid target." );
	}

	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buf_ptr
		+ sizeof( uint16_t ) // Message length.
		+ sizeof( uint8_t ) // Sender length.
		+ sizeof( uint8_t ) // Target length.
	);


	*reinterpret_cast<uint16_t*>( &buffer[buf_ptr] ) = static_cast<uint16_t>( m_message.size() ); buf_ptr += sizeof( uint16_t );
	buffer.insert( buffer.begin() + buf_ptr, m_message.c_str(), m_message.c_str() + m_message.size() ); buf_ptr += m_message.size();

	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_sender.size() ); buf_ptr += sizeof( uint8_t );
	buffer.insert( buffer.begin() + buf_ptr, m_sender.c_str(), m_sender.c_str() + m_sender.size() ); buf_ptr += m_sender.size();

	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_target.size() ); buf_ptr += sizeof( uint8_t );
	buffer.insert( buffer.begin() + buf_ptr, m_target.c_str(), m_target.c_str() + m_target.size() ); buf_ptr += m_target.size();
}

std::size_t Chat::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr = 0;

	// Message length.
	uint16_t message_length = 0;

	if( buffer_size - buf_ptr < sizeof( message_length ) ) {
		return 0;
	}

	message_length = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( message_length );

	if( message_length == 0 ) {
		throw BogusDataException( "Invalid message length." );
	}

	// Message pointer.
	const char* message_ptr = nullptr;

	if( buffer_size - buf_ptr < message_length ) {
		return 0;
	}

	message_ptr = &buffer[buf_ptr];
	buf_ptr += message_length;

	// Sender length.
	uint8_t sender_length = 0;

	if( buffer_size - buf_ptr < sizeof( sender_length ) ) {
		return 0;
	}

	sender_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( sender_length );

	if( sender_length == 0 ) {
		throw BogusDataException( "Invalid sender length." );
	}

	// Sender pointer.
	const char* sender_ptr = nullptr;

	if( buffer_size - buf_ptr < sender_length ) {
		return 0;
	}

	sender_ptr = &buffer[buf_ptr];
	buf_ptr += sender_length;

	// Target length.
	uint8_t target_length = 0;

	if( buffer_size - buf_ptr < sizeof( target_length ) ) {
		return 0;
	}

	target_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( target_length );

	if( target_length == 0 ) {
		throw BogusDataException( "Invalid target length." );
	}

	// Target pointer.
	const char* target_ptr = nullptr;

	if( buffer_size - buf_ptr < target_length ) {
		return 0;
	}

	target_ptr = &buffer[buf_ptr];
	buf_ptr += target_length;

	// Apply.
	m_message = std::string( message_ptr, message_length );
	m_sender = std::string( sender_ptr, sender_length );
	m_target = std::string( target_ptr, target_length );

	return buf_ptr;
}

void Chat::set_message( const std::string& message ) {
	m_message = message;
}

void Chat::set_sender( const std::string& sender ) {
	m_sender = sender;
}

void Chat::set_target( const std::string& target ) {
	m_target = target;
}

const std::string& Chat::get_message() const {
	return m_message;
}

const std::string& Chat::get_sender() const {
	return m_sender;
}

const std::string& Chat::get_target() const {
	return m_target;
}

}
}
