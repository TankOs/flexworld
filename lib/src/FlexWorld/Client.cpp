#include <FlexWorld/Client.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace flex {

void Client::Handler::handle_connect( ConnectionID id ) {
	std::cerr << "WARNING: Connection of #" << id << " not handled!" << std::endl;
}

void Client::Handler::handle_disconnect( ConnectionID id ) {
	std::cerr << "WARNING: Disconnection of #" << id << " not handled!" << std::endl;
}

Client::Client( boost::asio::io_service& io_service, Handler& handler ) :
	m_io_service( io_service ),
	m_handler( &handler ),
	m_started( false )
{
}

Client::~Client() {
	stop();
}

bool Client::is_started() const {
	return m_started;
}

bool Client::start( const std::string& ip, unsigned short port ) {
	using namespace boost::asio;

	if( m_started ) {
		return false;
	}

	m_started = true;

	// Make the connection.
	m_socket.reset( new ip::tcp::socket( m_io_service ) );

	m_socket->async_connect(
		ip::tcp::endpoint(
			ip::address_v4::from_string( ip ),
			port
		),
		boost::bind( &Client::handle_connect, this, placeholders::error )
	);

	return true;
}

void Client::handle_connect( const boost::system::error_code& error ) {
	// If connecting fails notify observer.
	if( error ) {
		m_handler->handle_disconnect( 0 );
		return;
	}

	// Notify observer.
	m_handler->handle_connect( 0 );

	// Connection succeeded, start to read data.
	start_read();
}

void Client::start_read() {
	m_socket->async_read_some(
		boost::asio::buffer(
			m_receive_buffer,
			READ_BUFFER_SIZE
		),
		boost::bind(
			&Client::handle_read,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void Client::handle_read( const boost::system::error_code& error, std::size_t num_bytes_read ) {
	if( error ) {
		m_handler->handle_disconnect( 0 );
		return;
	}

	// Append to buffer.
	m_buffer.insert( m_buffer.end(), m_receive_buffer, m_receive_buffer + num_bytes_read );

	// Dispatch until buffer empty.
	std::size_t consumed = 0;

	while( m_buffer.size() > 1 && (consumed = ServerProtocol::dispatch( m_buffer, *m_handler, 0 )) > 0 ) {
		if( consumed == m_buffer.size() ) {
			m_buffer.clear();
		}
		else {
			std::memmove( &m_buffer[0], &m_buffer[0] + consumed, m_buffer.size() - consumed );
			m_buffer.resize( m_buffer.size() - consumed );
		}
	}

	// Start another read.
	start_read();
}

void Client::handle_write( const boost::system::error_code& error, std::shared_ptr<ServerProtocol::Buffer> /*buffer*/ ) {
	if( error ) {
		// Sending failed, stop client.
		assert( 0 && "Sending failed?!" );
		return;
	}
}

void Client::set_handler( Handler& handler ) {
	m_handler = &handler;
}

Client::Handler& Client::get_handler() {
	return *m_handler;
}

void Client::stop() {
	if( m_socket ) {
		m_socket->close();
	}

	m_started = false;
}

}
