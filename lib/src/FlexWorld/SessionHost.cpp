#include <FlexWorld/SessionHost.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/Messages/Beam.hpp>
#include <FlexWorld/Messages/Chunk.hpp>
#include <FlexWorld/Messages/CreateEntity.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Log.hpp>
#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/Account.hpp>
#include <FlexWorld/World.hpp>
#include <FlexWorld/GameMode.hpp>
#include <FlexWorld/PackageEnumerator.hpp>

#include <boost/filesystem.hpp>
#include <set>

namespace flex {

static const Chunk::Vector DEFAULT_CHUNK_SIZE = Chunk::Vector( 16, 16, 16 );
static const Planet::Vector DEFAULT_CONSTRUCT_SIZE = Planet::Vector( 16, 16, 16 );

SessionHost::SessionHost(
	boost::asio::io_service& io_service,
	LockFacility& lock_facility,
	AccountManager& account_manager,
	World& world,
	const GameMode& game_mode
) :
	m_game_mode( game_mode ),
	m_num_loaded_scripts( 0 ),
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
	// Check that we have search paths.
	if( m_class_loader.get_num_search_paths() < 1 ) {
		Log::Logger( Log::FATAL ) << "No search paths." << Log::endl;
		return false;
	}

	// Lock world for whole boot-up process, as we're creating a planet and
	// loading classes.
	m_lock_facility.lock_world( true );

	// Load required grass class.
	const Class* grass_cls = get_or_load_class( FlexID::make( "fw.base.nature/grass" ) );

	if( grass_cls == nullptr ) {
		Log::Logger( Log::FATAL ) << "Failed to load grass class." << Log::endl;
		m_lock_facility.lock_world( false );
		return false;
	}

	// Load required default entity class.
	if( get_or_load_class( m_game_mode.get_default_entity_class_id() ) == nullptr ) {
		Log::Logger( Log::FATAL ) << "Failed to load default entity class." << Log::endl;
		m_lock_facility.lock_world( false );
		return false;
	}

	// Construct planet.
	if( m_world.find_planet( "construct" ) != nullptr ) {
		Log::Logger( Log::FATAL ) << "Planet \"construct\" does already exist." << Log::endl;
		m_lock_facility.lock_world( false );
		return false;
	}

	m_world.create_planet( "construct", DEFAULT_CONSTRUCT_SIZE, DEFAULT_CHUNK_SIZE );

	Planet* planet = m_world.find_planet( "construct" );
	if( !planet ) {
		Log::Logger( Log::FATAL ) << "Failed to create \"construct\"." << Log::endl;
		m_lock_facility.lock_world( false );
		return false;
	}

	// Build grass plane.
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
	}

	// Release lock again.
	m_lock_facility.lock_world( false );

	// Load scripts.
	rehash_scripts();

	return m_server->start();
}

bool SessionHost::is_running() const {
	return m_server->is_running();
}

void SessionHost::handle_connect( Server::ConnectionID conn_id ) {
	Log::Logger( Log::INFO ) << "Client connected from " << m_server->get_client_ip( conn_id ) << "." << Log::endl;

	// Prepare player info.
	if( conn_id >= m_player_infos.size() ) {
		m_player_infos.resize( conn_id + 1 );
	}

	assert( m_player_infos[conn_id].connected == false );
	m_player_infos[conn_id].connected = true;

	// Check limit (important: player info must be created before, because handle_disconnect checks it).
	if( m_server->get_num_peers() > m_player_limit ) {
		Log::Logger( Log::WARNING ) << "Server full, disconnecting " << m_server->get_client_ip( conn_id ) << "." << Log::endl;
		m_server->disconnect_client( conn_id );
		return;
	}

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

	// If it doesn't exist, create a new one.
	if( account == nullptr ) {
		// Create entity.
		m_lock_facility.lock_world( true );

		assert( m_world.find_class( m_game_mode.get_default_entity_class_id() ) != nullptr );

		const Entity& entity = m_world.create_entity( m_game_mode.get_default_entity_class_id() );

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

	beam_player( conn_id, "construct", sf::Vector3f( 40, 2.75f, 40 ), 200 );
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
	info.entity->set_rotation( sf::Vector3f( 0, heading, 0 ) );

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

	// Send message.
	m_server->send_message( beam_msg, conn_id );

	// Send entities.
	// TODO Send only entities in vicinity.
	std::size_t num_entities = planet->get_num_entities();
	Entity* entity = nullptr;

	for( std::size_t entity_idx = 0 ; entity_idx < num_entities; ++entity_idx ) {
		entity = m_world.find_entity( planet->get_entity_id( entity_idx ) );

		if( !entity ) {
			Log::Logger( Log::ERR )
				<< "Planet " << planet->get_id() << " references non-existant entity #"
				<< planet->get_entity_id( entity_idx ) << "." << Log::endl
			;
			continue;
		}

		msg::CreateEntity create_msg;
		create_msg.set_position( entity->get_position() );
		create_msg.set_heading( entity->get_rotation().y );
		create_msg.set_class( entity->get_class().get_id().get() );

		m_server->send_message( create_msg, conn_id );
	}

	m_lock_facility.lock_world( false );
	m_lock_facility.lock_planet( *planet, false );
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

const Class* SessionHost::get_or_load_class( const FlexID& id ) {
	m_lock_facility.lock_world( true );

	// At first check if the class is already present.
	const Class* cls = m_world.find_class( id );

	// If not, try to load it using the class loader.
	if( cls == nullptr ) {
		try {
			Class new_cls = m_class_loader.load( id );

			// Loaded, add to world and store reference.
			m_world.add_class( new_cls );

			cls = m_world.find_class( id );
			assert( cls != nullptr );
		}
		catch( const ClassLoader::LoadException& e ) {
			Log::Logger( Log::ERR ) << "Failed to load class " << id.get() << ". Reason: " << e.what() << Log::endl;
			return nullptr;
		}
	}

	m_lock_facility.lock_world( false );

	return cls;
}

void SessionHost::add_search_path( const std::string& path ) {
	assert( boost::filesystem::exists( path ) );

	if( !boost::filesystem::exists( path ) ) {
		Log::Logger( Log::ERR ) << "Search path doesn't exist: " << path << Log::endl;
	}
	else {
		m_class_loader.add_search_path( path );
	}
}

std::size_t SessionHost::get_num_loaded_scripts() const {
	return m_num_loaded_scripts;
}

void SessionHost::rehash_scripts() {
	Log::Logger( Log::INFO ) << "Searching for scripts in " << m_game_mode.get_num_packages() << " package(s)." << Log::endl;

	// Clear current script manager. TODO Trigger UNLOAD_EVENT.
	m_script_manager.clear();
	m_num_loaded_scripts = 0;

	// Enumerate scripts of game mode.
	PackageEnumerator enumerator;
	std::set<std::string> found_scripts;

	for( std::size_t package_idx = 0; package_idx < m_game_mode.get_num_packages(); ++package_idx ) {
		const FlexID& package = m_game_mode.get_package( package_idx );
		std::string package_path = package.as_path();

		//Log::Logger( Log::INFO ) << "-> " << package.get() << Log::endl;

		// Search package in all search paths, also process all paths, i.e. load
		// scripts from ALL packages in ALL search paths.
		for( std::size_t path_idx = 0; path_idx < m_class_loader.get_num_search_paths(); ++path_idx ) {
			std::string full_package_path = m_class_loader.get_search_path( path_idx ) + package_path;

			// Enumerate.
			if( enumerator.enumerate( full_package_path ) ) {
				Log::Logger( Log::INFO ) << "-> " << full_package_path << " (" << enumerator.get_num_script_files() << " script(s))" << Log::endl;

				// Select scripts.
				for( std::size_t script_idx = 0; script_idx < enumerator.get_num_script_files(); ++script_idx ) {
					// Add to set so the same scripts won't be loaded twice. This may
					// happen if the user did a mistake and specified both a package and
					// one or more of its sub-package (e.g. "foo.bar" and "foo").
					found_scripts.insert( enumerator.get_script_file( script_idx ) );
				}
			}
		}
	}

	// Load scripts.
	if( found_scripts.size() == 0 ) {
		Log::Logger( Log::WARNING ) << "No scripts loaded." << Log::endl;
	}
	else {
		Log::Logger( Log::INFO ) << "Loading scripts..." << Log::endl;

		std::set<std::string>::iterator script_path_iter( found_scripts.begin() );
		std::set<std::string>::iterator script_path_iter_end( found_scripts.end() );
		
		for( ; script_path_iter != script_path_iter_end; ++script_path_iter ) {
			if( m_script_manager.execute_file( *script_path_iter ) ) {
				// Output filename only.
				Log::Logger( Log::INFO ) << "-> " << *script_path_iter << Log::endl;
				++m_num_loaded_scripts;
			}
			else {
				Log::Logger( Log::ERR ) << "Failed executing script: " << *script_path_iter << Log::endl;
				Log::Logger( Log::ERR ) << m_script_manager.get_last_error() << Log::endl;
			}
		}
	}
}

void SessionHost::handle_message( const msg::Chat& chat_msg, Server::ConnectionID conn_id ) {
	assert( conn_id < m_player_infos.size() );

	// Check if the sent message is a command.
	const sf::String& text = chat_msg.get_message();

	if( text[0] == '/' ) {
		// Extract arguments.
		std::vector<sf::String> args;

		std::size_t pos = 1; // Skip slash.
		std::string command;
		sf::String token;

		while( pos < text.getSize() ) {
			if( text[pos] != ' ' ) {
				token += text[pos];
			}

			// If delimiter found or at last position, extract command/argument.
			if( (text[pos] == ' ' || pos + 1 == text.getSize()) && token.getSize() > 0 ) {
				// If command not extracted yet do it now.
				if( command.empty() ) {
					command = static_cast<std::string>( token );
				}
				else { // Extract argument.
					args.push_back( token );
				}

				token.clear();
			}

			++pos;
		}

		// Give to script manager.
		if( command.empty() == false ) {
			m_script_manager.trigger_command( command, args );
		}
	}
	else {
		// No command, give to script manager normally.
		m_script_manager.trigger_chat_system_event( chat_msg.get_message(), chat_msg.get_channel(), conn_id );
	}
}

}
