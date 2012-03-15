#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/Messages/Beam.hpp>
#include <FlexWorld/Messages/Chunk.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Log.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/Account.hpp>
#include <FlexWorld/World.hpp>

namespace flex {

static const Chunk::Vector DEFAULT_CHUNK_SIZE = Chunk::Vector( 16, 16, 16 );
static const Planet::Vector DEFAULT_CONSTRUCT_SIZE = Planet::Vector( 16, 16, 16 );

SessionHost::SessionHost(
	boost::asio::io_service& io_service,
	LockFacility& lock_facility,
	AccountManager& account_manager,
	World& world
) :
	m_io_service( io_service ),
	m_lock_facility( lock_facility ),
	m_account_manager( account_manager ),
	m_world( world ),
	m_auth_mode( OPEN_AUTH ),
	m_player_limit( 1 ),
	m_max_view_radius( 10 )
{
	m_server.reset( new Server( m_io_service, *this ) );
}

SessionHost::~SessionHost() {
	stop();
}

const LockFacility& SessionHost::get_lock_facility() const {
	return m_lock_facility;
}

const AccountManager& SessionHost::get_account_manager() const {
	return m_account_manager;
}

void SessionHost::set_ip( const std::string& ip ) {
	m_server->set_ip( ip );
}

void SessionHost::set_port( unsigned short port ) {
	m_server->set_port( port );
}

const std::string& SessionHost::get_ip() const {
	return m_server->get_ip();
}

unsigned short SessionHost::get_port() const {
	return m_server->get_port();
}

bool SessionHost::start() {
	// Make sure fw.base.nature/grass is present for construction the planet
	// "construct".
	FlexID id = FlexID::make( "fw.base.nature/grass" );

	const Class* grass_cls = m_world.find_class( id );
	if( !grass_cls ) {
		Log::Logger( Log::FATAL ) << id.get() << " doesn't exist but needed for planet \"construct\"." << Log::endl;
		return false;
	}

	// Construct planet.
	if( m_world.find_planet( "construct" ) != nullptr ) {
		Log::Logger( Log::FATAL ) << "Planet \"construct\" does already exist." << Log::endl;
		return false;
	}

	m_world.create_planet( "construct", DEFAULT_CONSTRUCT_SIZE, DEFAULT_CHUNK_SIZE );

	Planet* planet = m_world.find_planet( "construct" );
	if( !planet ) {
		Log::Logger( Log::FATAL ) << "Failed to create \"construct\"." << Log::endl;
		return false;
	}

	// Create grass plane.
	{
		Planet::Vector chunk_pos( 0, 0, 0 );
		Chunk::Vector block_pos( 0, 0, 0 );

		for( chunk_pos.x = 0; chunk_pos.x < DEFAULT_CONSTRUCT_SIZE.x; ++chunk_pos.x ) {
			for( chunk_pos.z = 0; chunk_pos.z < DEFAULT_CONSTRUCT_SIZE.z; ++chunk_pos.z ) {
				// Create chunk
				if( !planet->has_chunk( chunk_pos ) ) {
					planet->create_chunk( chunk_pos );
				}

				for( block_pos.x = 0; block_pos.x < DEFAULT_CHUNK_SIZE.x; ++block_pos.x ) {
					for( block_pos.z = 0; block_pos.z < DEFAULT_CHUNK_SIZE.z; ++block_pos.z ) {
						planet->set_block( chunk_pos, block_pos, *grass_cls );
					}
				}
			}
		}

		// Some test blocks. XXX
		FlexID stone_id = FlexID::make( "fw.base.nature/stone" );
		const Class* stone_cls = m_world.find_class( stone_id );

		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 0, 1, 0 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 1, 1, 0 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 2, 1, 0 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 3, 1, 0 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 4, 1, 0 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 0, 1, 4 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 1, 1, 4 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 2, 1, 4 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 3, 1, 4 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 4, 1, 4 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 0, 1, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 0, 1, 2 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 0, 1, 3 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 4, 1, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 4, 1, 2 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 4, 1, 3 ), *stone_cls );

		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 1, 2, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 2, 2, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 3, 2, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 1, 2, 3 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 2, 2, 3 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 3, 2, 3 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 1, 2, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 1, 2, 2 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 3, 2, 1 ), *stone_cls );
		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 3, 2, 2 ), *stone_cls );

		planet->set_block( Planet::Vector( 3, 0, 2 ), Chunk::Vector( 2, 3, 2 ), *stone_cls );
	}

	return m_server->start();
}

bool SessionHost::is_running() const {
	return m_server->is_running();
}

void SessionHost::handle_connect( Server::ConnectionID conn_id ) {
	Log::Logger( Log::INFO ) << "Client connected from " << m_server->get_client_ip( conn_id ) << "." << Log::endl;

	// Check limit.
	if( m_server->get_num_peers() > m_player_limit ) {
		Log::Logger( Log::WARNING ) << "Server full, disconnecting " << m_server->get_client_ip( conn_id ) << "." << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

	// Prepare player info.
	if( conn_id >= m_player_infos.size() ) {
		m_player_infos.resize( conn_id + 1 );
	}

	assert( m_player_infos[conn_id].connected == false );
	m_player_infos[conn_id].connected = true;

	// Client connected, send server info.
	msg::ServerInfo msg;
	msg.set_auth_mode( m_auth_mode == OPEN_AUTH ? msg::ServerInfo::OPEN_AUTH : msg::ServerInfo::KEY_AUTH );
	msg.set_flags( msg::ServerInfo::NO_FLAGS );

	m_server->send_message( msg, conn_id );
}

void SessionHost::handle_disconnect( Server::ConnectionID conn_id ) {
	assert( conn_id < m_player_infos.size() );
	assert( m_player_infos[conn_id].connected == true );

	// Reset player info.
	m_player_infos[conn_id] = PlayerInfo();

	Log::Logger( Log::INFO ) << "Client " << m_server->get_client_ip( conn_id ) << " disconnected." << Log::endl;
}

void SessionHost::handle_message( const msg::OpenLogin& login_msg, Server::ConnectionID conn_id ) {
	const std::string& remote_ip = m_server->get_client_ip( conn_id );

	// If we're not in open auth mode, disconnect client.
	if( m_auth_mode != OPEN_AUTH ) {
		Log::Logger( Log::INFO ) << "Client " << remote_ip << " tried to open-auth, but we're setup for key auth." << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

	bool is_local = false;

	// Check if client is a local client.
	if( m_server->get_client_ip( conn_id ) == "127.0.0.1" ) {
		is_local = true;
	}

	if( !is_local ) {
		Log::Logger( Log::INFO ) << "Client wants to login as " << login_msg.get_username() << "." << Log::endl;
	}
	else {
		Log::Logger( Log::INFO ) << "Client " << login_msg.get_username() << " is connecting from this machine, authenticated." << Log::endl;
	}

	m_player_infos[conn_id].local = is_local;

	m_lock_facility.lock_account_manager( true );

	// Check if an account for that username exists.
	const flex::Account* account = m_account_manager.find_account( login_msg.get_username() );

	// If it doesn't exist, create a new one. TODO: Make this configurable?
	if( account == nullptr ) {
		// Create entity.
		m_lock_facility.lock_world( true );
		const Entity& entity = m_world.create_entity( FlexID::make( "fw.base.human/dwarf_male" ) ); // TODO: Change!

		Account new_account;
		new_account.set_username( login_msg.get_username() );
		new_account.set_password( login_msg.get_password() );
		new_account.set_entity_id( entity.get_id() );
		m_account_manager.add_account( new_account );

		m_lock_facility.lock_world( false );

		account = m_account_manager.find_account( login_msg.get_username() );
		assert( account != nullptr );

		Log::Logger( Log::INFO ) << "New account created: " << login_msg.get_username() << " (ent#" << account->get_entity_id() << ")" << Log::endl;
	}
	else {
		// Check for correct password.
		if( account->get_password() != login_msg.get_password() ) {
			Log::Logger( Log::INFO ) << "Client " << remote_ip << " gave wrong password for account " << login_msg.get_password() << "." << Log::endl;
			m_server->disconnect_client( conn_id );
			return;
		}
	}

	// Remember entity ID before lock is released.
	Entity::ID entity_id = account->get_entity_id();
	m_lock_facility.lock_account_manager( false );

	// Remember username.
	m_player_infos[conn_id].username = login_msg.get_username();

	// Associate entity.
	m_lock_facility.lock_world( true );

	Entity* entity = m_world.find_entity( entity_id );
	assert( entity != nullptr );

	m_player_infos[conn_id].entity = entity;

	m_lock_facility.lock_world( false );

	// Everything is good, log the player in!
	msg::LoginOK ok_msg;
	ok_msg.set_entity_id( entity_id );
	m_server->send_message( ok_msg, conn_id );
}

void SessionHost::set_auth_mode( AuthMode mode ) {
	m_auth_mode = mode;
}

SessionHost::AuthMode SessionHost::get_auth_mode() const {
	return m_auth_mode;
}

const World& SessionHost::get_world() const {
	return m_world;
}

void SessionHost::set_player_limit( std::size_t limit ) {
	assert( !is_running() );
	m_player_limit = limit;
}

std::size_t SessionHost::get_player_limit() const {
	return m_player_limit;
}

void SessionHost::handle_message( const msg::Ready& /*login_msg*/, Server::ConnectionID conn_id ) {
	// Get construct.
	m_lock_facility.lock_world( true );

	Planet* construct = m_world.find_planet( "construct" );

	if( construct ) {
		m_lock_facility.lock_planet( *construct, true );
	}

	m_lock_facility.lock_world( false );

	if( !construct ) {
		Log::Logger( Log::FATAL ) << "Planet construct is unavailable!" << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

	// Client is ready, send him to the construct planet.
	m_lock_facility.lock_planet( *construct, false );

	// Beam.
	beam_player( conn_id, "construct", sf::Vector3f( 0, 0, 0 ), 0 );
}

void SessionHost::beam_player( Server::ConnectionID conn_id, const std::string& planet_id, const sf::Vector3f& position, float heading ) {
	assert( conn_id < m_player_infos.size() && m_player_infos[conn_id].connected == true );
	assert( !planet_id.empty() );
	assert( position.x >= 0 && position.y >= 0 && position.z >= 0 );

	// Get player info.
	PlayerInfo& info = m_player_infos[conn_id];

	// Get planet.
	m_lock_facility.lock_world( true );

	Planet* planet = m_world.find_planet( planet_id );
	assert( planet != nullptr );

	m_lock_facility.lock_planet( *planet, true );

	// Update entity.
	info.entity->set_position( position );
	// TODO Angle/heading
	// TODO Notify players on old planet of entity teleport.

	// Link entity to new planet.
	m_world.link_entity_to_planet( info.entity->get_id(), planet_id );

	// Save current planet.
	info.planet = planet;

	// Construct beam message.
	msg::Beam beam_msg;
	beam_msg.set_planet_name( planet->get_id() );
	beam_msg.set_chunk_size( planet->get_chunk_size() );
	beam_msg.set_planet_size( planet->get_size() );
	beam_msg.set_position( position );
	beam_msg.set_heading( heading );

	m_lock_facility.lock_world( false );

	// Transform coord.
	Planet::Vector chunk_pos( 0, 0, 0 );
	Chunk::Vector block_pos( 0, 0, 0 );

	bool result = planet->transform( position, chunk_pos, block_pos );
	assert( result == true );

	// Update view cuboid.
	info.view_cuboid.x = static_cast<Planet::ScalarType>( chunk_pos.x - std::min( chunk_pos.x, m_max_view_radius ) );
	info.view_cuboid.y = static_cast<Planet::ScalarType>( chunk_pos.y - std::min( chunk_pos.y, m_max_view_radius ) );
	info.view_cuboid.z = static_cast<Planet::ScalarType>( chunk_pos.z - std::min( chunk_pos.z, m_max_view_radius ) );
	info.view_cuboid.width = std::min( static_cast<Planet::ScalarType>( planet->get_size().x - chunk_pos.x ), m_max_view_radius );
	info.view_cuboid.height = std::min( static_cast<Planet::ScalarType>( planet->get_size().y - chunk_pos.y ), m_max_view_radius );
	info.view_cuboid.depth = std::min( static_cast<Planet::ScalarType>( planet->get_size().z - chunk_pos.z ), m_max_view_radius );

	m_lock_facility.lock_planet( *planet, false );

	// Send message.
	m_server->send_message( beam_msg, conn_id );
}

void SessionHost::handle_message( const msg::RequestChunk& req_chunk_msg, Server::ConnectionID conn_id ) {
	PlayerInfo& info = m_player_infos[conn_id];

	// Make sure client is at a planet.
	if( info.planet == nullptr ) {
		Log::Logger( Log::ERR ) << "Client #" << conn_id << " requested a chunk but isn't on a planet." << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

	// Check for valid chunk position.
	if(
		req_chunk_msg.get_position().x >= info.planet->get_size().x ||
		req_chunk_msg.get_position().y >= info.planet->get_size().y ||
		req_chunk_msg.get_position().z >= info.planet->get_size().z
	) {
		Log::Logger( Log::ERR ) << "Client #" << conn_id << " requested an invalid chunk." << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

	// Check if chunk exists.
	if( !info.planet->has_chunk( req_chunk_msg.get_position() ) ) {
		// TODO: Send EmptyChunk msg.
	}
	else {
		// Check if chunk hasn't changed or client is connected from the local
		// machine (so that it uses the same backend).
		// TODO: Timestamp check.
		if( info.local ) {
			msg::ChunkUnchanged unch_msg;
			unch_msg.set_position( req_chunk_msg.get_position() );
			m_server->send_message( unch_msg, conn_id );
		}
	}
}

void SessionHost::stop() {
	m_server->stop();
}

}
