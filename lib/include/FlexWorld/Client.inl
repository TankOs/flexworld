#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

namespace flex {

template <class MsgType>
void Client::send_message( const MsgType& message ) {
	assert( m_connected );
	assert( m_socket );

	if( !m_connected || !m_socket ) {
		return;
	}

	std::shared_ptr<ServerProtocol::Buffer> buffer( new ServerProtocol::Buffer );
	ServerProtocol::serialize_message( message, *buffer );

	m_socket->async_send(
		boost::asio::buffer(
			&buffer->front(),
			buffer->size()
		),
		boost::bind(
			&Client::handle_write,
			this,
			boost::asio::placeholders::error,
			buffer
		)
	);
}

}
