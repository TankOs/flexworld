#include <FlexWorld/Client.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace flex {

void Client::Handler::handle_connect( ConnectionID id ) {
	std::cerr << "WARNING: Connection of #" << id << " not handled!" << std::endl;
}

void Client::Handler::handle_disconnect( ConnectionID id ) {
	std::cerr << "WARNING: Disconnection of #" << id << " not handled!" << std::endl;
}

Client::Client( Handler& handler ) :
	m_handler( handler ),
	m_connected( false )
{
}

Client::~Client() {
}

bool Client::is_connected() const {
	return m_connected;
}

bool Client::run( const std::string& ip, unsigned short port ) {
	using namespace boost::asio;

	if( m_connected ) {
		return false;
	}

	m_connected = true;

	// Make the connection.
	m_socket.reset( new ip::tcp::socket( m_io_service ) );

	m_socket->async_connect(
		ip::tcp::endpoint(
			ip::address_v4::from_string( ip ),
			port
		),
		boost::bind( &Client::handle_connect, this, placeholders::error )
	);

	// Run I/O service.
	m_io_service.run();

	// Cleanup.
	m_socket.reset();
	m_connected = false;

	return true;
}

void Client::handle_connect( const boost::system::error_code& error ) {
	// If connecting fails notify observer.
	if( error ) {
		m_connected = false;
		m_handler.handle_disconnect( 0 );
		return;
	}

	// Notify observer.
	m_handler.handle_connect( 0 );

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
		m_handler.handle_disconnect( 0 );
		return;
	}

	// Append to buffer.
	m_buffer.insert( m_buffer.end(), m_receive_buffer, m_receive_buffer + num_bytes_read );

	// Dispatch until buffer empty.
	std::size_t consumed = 0;

	while( m_buffer.size() && (consumed = ServerProtocol::dispatch( m_buffer, m_handler, 0 )) > 0 ) {
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

void Client::stop() {
	assert( m_connected );
	m_io_service.stop();
}

void Client::handle_write( const boost::system::error_code& error, std::shared_ptr<ServerProtocol::Buffer> /*buffer*/ ) {
	if( error ) {
		// Sending failed, stop client.
		stop();
		return;
	}
}

}
