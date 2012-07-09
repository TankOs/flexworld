namespace fw {

template <class MsgType>
void Server::send_message( const MsgType& message, ConnectionID conn_id ) {
	assert( conn_id < m_peers.size() );
	assert( m_peers[conn_id] != nullptr );

	std::shared_ptr<ServerProtocol::Buffer> buffer( new ServerProtocol::Buffer );
	ServerProtocol::serialize_message( message, *buffer );

	m_peers[conn_id]->socket->async_send(
		boost::asio::buffer(
			&buffer->front(),
			buffer->size()
		),
		boost::bind(
			&Server::handle_write,
			this,
			boost::asio::placeholders::error,
			buffer,
			conn_id
		)
	);
}

}
