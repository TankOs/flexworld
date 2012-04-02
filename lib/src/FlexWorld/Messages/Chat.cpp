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
	if( m_message.isEmpty() || m_message.getSize() > 0xffff ) {
		throw InvalidDataException( "Invalid message." );
	}

	if( m_sender.isEmpty() || m_sender.getSize() > 0xff ) {
		throw InvalidDataException( "Invalid sender." );
	}

	if( m_channel.isEmpty() || m_channel.getSize() > 0xff ) {
		throw InvalidDataException( "Invalid channel." );
	}

	std::size_t buf_ptr = buffer.size();

	buffer.resize(
		+ buf_ptr
		+ sizeof( uint16_t ) // Message length.
		+ sizeof( uint8_t ) // Channel length.
		+ sizeof( uint8_t ) // Sender length.
	);


	// Message.
	*reinterpret_cast<uint16_t*>( &buffer[buf_ptr] ) = static_cast<uint16_t>( m_message.getSize() );
	buf_ptr += sizeof( uint16_t );

	buffer.insert(
		buffer.begin() + buf_ptr,
		reinterpret_cast<const char*>( m_message.getData() ),
		reinterpret_cast<const char*>( m_message.getData() ) + (m_message.getSize() * sizeof( sf::Uint32 ))
	);
	buf_ptr += sizeof( sf::Uint32 ) * m_message.getSize();

	// Channel.
	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_channel.getSize() );
	buf_ptr += sizeof( uint8_t );

	buffer.insert(
		buffer.begin() + buf_ptr,
		reinterpret_cast<const char*>( m_channel.getData() ),
		reinterpret_cast<const char*>( m_channel.getData() ) + (m_channel.getSize() * sizeof( sf::Uint32 ))
	);
	buf_ptr += sizeof( sf::Uint32 ) * m_channel.getSize();

	// Sender.
	*reinterpret_cast<uint8_t*>( &buffer[buf_ptr] ) = static_cast<uint8_t>( m_sender.getSize() );
	buf_ptr += sizeof( uint8_t );

	buffer.insert(
		buffer.begin() + buf_ptr,
		reinterpret_cast<const char*>( m_sender.getData() ),
		reinterpret_cast<const char*>( m_sender.getData() ) + (m_sender.getSize() * sizeof( sf::Uint32 ))
	);
	buf_ptr += sizeof( sf::Uint32 ) * m_sender.getSize();
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
	const sf::Uint32* message_ptr = nullptr;

	if( buffer_size - buf_ptr < message_length * sizeof( sf::Uint32 ) ) {
		return 0;
	}

	message_ptr = reinterpret_cast<const sf::Uint32*>( &buffer[buf_ptr] );
	buf_ptr += message_length * sizeof( sf::Uint32 );

	// Channel length.
	uint8_t channel_length = 0;

	if( buffer_size - buf_ptr < sizeof( channel_length ) ) {
		return 0;
	}

	channel_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( channel_length );

	if( channel_length == 0 ) {
		throw BogusDataException( "Invalid channel length." );
	}

	// Channel pointer.
	const sf::Uint32* channel_ptr = nullptr;

	if( buffer_size - buf_ptr < channel_length * sizeof( sf::Uint32 ) ) {
		return 0;
	}

	channel_ptr = reinterpret_cast<const sf::Uint32*>( &buffer[buf_ptr] );
	buf_ptr += channel_length * sizeof( sf::Uint32 );

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
	const sf::Uint32* sender_ptr = nullptr;

	if( buffer_size - buf_ptr < sender_length * sizeof( sf::Uint32 ) ) {
		return 0;
	}

	sender_ptr = reinterpret_cast<const sf::Uint32*>( &buffer[buf_ptr] );
	buf_ptr += sender_length * sizeof( sf::Uint32 );

	// Apply.
	m_message = std::basic_string<sf::Uint32>( message_ptr, message_length );
	m_sender = std::basic_string<sf::Uint32>( sender_ptr, sender_length );
	m_channel = std::basic_string<sf::Uint32>( channel_ptr, channel_length );

	return buf_ptr;
}

void Chat::set_message( const sf::String& message ) {
	m_message = message;
}

void Chat::set_sender( const sf::String& sender ) {
	m_sender = sender;
}

void Chat::set_channel( const sf::String& channel ) {
	m_channel = channel;
}

const sf::String& Chat::get_message() const {
	return m_message;
}

const sf::String& Chat::get_sender() const {
	return m_sender;
}

const sf::String& Chat::get_channel() const {
	return m_channel;
}

}
}
