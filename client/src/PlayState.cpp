#include <GL/glew.h>

#include "PlayState.hpp"
#include "MenuState.hpp"
#include "Shared.hpp"

#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Math.hpp>
#include <FlexWorld/Config.hpp>
#include <sstream>

#include "BufferObject.hpp" // XXX 

PlayState::PlayState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target ),
	m_console( Console::Create() ),
	m_wireframe( false ),
	m_view_cuboid( 0, 0, 0, 1, 1, 1 ),
	m_do_prepare_chunks( false ),
	m_velocity( 0, 0, 0 ),
	m_update_velocity( false ),
	m_walk_forward( false ),
	m_walk_backward( false ),
	m_strafe_left( false ),
	m_strafe_right( false )
{
}

void PlayState::init() {
	// Init GLEW.
	//glewInit();

	set_logic_fps( 120 );
	set_render_fps( static_cast<uint16_t>( get_shared().user_settings.get_fps_limit() ) );

	// Reset handler.
	get_shared().client->set_handler( *this );

	// Setup console.
	m_desktop.Add( m_console );

	m_console->SetAllocation(
		sf::FloatRect(
			0,
			0,
			static_cast<float>( get_render_target().GetWidth() ),
			static_cast<float>( get_render_target().GetHeight() ) * .3f
		)
	);

	m_console->add_message( "Press F11 to show/hide the console." );
	m_console->Show( false );

	// Setup UI.
	m_fps_text.SetCharacterSize( 12 );

	// Setup scene.
	// Sky.
	m_sky.reset( new Sky );
	m_sky->set_camera( m_camera );

	m_sun_texture.LoadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/sky/sun.png" ) );
	m_sun_texture.SetSmooth( true );
	m_sky->set_sun_texture( m_sun_texture );

	// Setup camera.
	m_camera.set_fov( 70.0f );
	m_camera.set_aspect_ratio(
		static_cast<float>( get_render_target().GetWidth() ) / static_cast<float>( get_render_target().GetHeight() )
	);
	m_camera.set_pitch_clamp( 90.f );

	// Projection matrix.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	gluPerspective(
		m_camera.get_fov(),
		m_camera.get_aspect_ratio(),
		0.1f,
		150.0f
	);
	
	// Texture matrix.
	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );

	// Misc.
	get_render_target().ShowMouseCursor( false );

	// Notify server that we're ready.
	flex::msg::Ready ready_msg;
	get_shared().client->send_message( ready_msg );
}

void PlayState::cleanup() {
	// Terminate chunk preparation thread.
	stop_and_wait_for_chunk_preparation_thread();

	// Close connections and wait for threads to complete.
	if( get_shared().client->is_connected() ) {
		get_shared().client->stop();
	}

	if( get_shared().client_thread ) {
		get_shared().client_thread->join();
	}

	if( get_shared().host ) {
		if( get_shared().host->is_running() ) {
			get_shared().host->stop();
		}

		if( get_shared().host_thread ) {
			get_shared().host_thread->join();
		}
	}

	// Free all backend stuff.
	get_shared().client.reset();
	get_shared().lock_facility.reset();
	get_shared().client_thread.reset();
	get_shared().host_thread.reset();

	// Restore old matrices.
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_TEXTURE );
	glPopMatrix();

	get_render_target().ShowMouseCursor( true );
}

void PlayState::handle_event( const sf::Event& event ) {
	bool give_gui = true;

	if(
		event.Type == sf::Event::Closed ||
		(event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::Escape)
	) {
		leave( new MenuState( get_render_target() ) );
		give_gui = false;
	}

	if( event.Type == sf::Event::KeyPressed ) {
		if( event.Key.Code == sf::Keyboard::F11 ) {
			m_console->Show( !m_console->IsVisible() );
			give_gui = false;
		}
		else if( event.Key.Code == sf::Keyboard::F3 ) {
			m_wireframe = !m_wireframe;
			give_gui = false;
		}
	}

	// Check for movement keys.
	if( event.Type == sf::Event::KeyPressed || event.Type == sf::Event::KeyReleased ) {
		Controls::Action action = get_shared().user_settings.get_controls().get_key_action( event.Key.Code );

		if( action != Controls::UNMAPPED ) {
			bool pressed = (event.Type == sf::Event::KeyPressed) ? true : false;
			give_gui = false;

			if( action == Controls::WALK_FORWARD ) {
				m_walk_forward = pressed;
				m_update_velocity = true;
			}
			else if( action == Controls::WALK_BACKWARD ) {
				m_walk_backward = pressed;
				m_update_velocity = true;
			}
			else if( action == Controls::STRAFE_LEFT ) {
				m_strafe_left = pressed;
				m_update_velocity = true;
			}
			else if( action == Controls::STRAFE_RIGHT ) {
				m_strafe_right = pressed;
				m_update_velocity = true;
			}
		}

	}

	if( give_gui ) {
		m_desktop.HandleEvent( event );
	}
}

void PlayState::update( const sf::Time& delta ) {
	// Update sky.
	//m_sky->set_time_of_day( m_sky->get_time_of_day() + (delta.AsSeconds() * (0.041f / 30.0f)) );
	m_sky->set_time_of_day( 0.1f );

	// Update GUI.
	m_desktop.Update( delta.AsSeconds() );

	// Get mouse movement.
	sf::Vector2i mouse_delta(
		sf::Mouse::GetPosition( get_render_target() ) -
		sf::Vector2i( get_render_target().GetWidth() / 2, get_render_target().GetHeight() / 2 )
	);

	// Rotate camera.
	m_camera.turn(
		sf::Vector3f(
			(
			 (get_shared().user_settings.get_controls().is_mouse_inverted() ? -1.0f : 1.0f) *
			 static_cast<float>( mouse_delta.y ) * get_shared().user_settings.get_controls().get_mouse_sensitivity()
			),
			static_cast<float>( mouse_delta.x ) * get_shared().user_settings.get_controls().get_mouse_sensitivity(),
			0.0f
		)
	);

	// Reset mouse.
	sf::Mouse::SetPosition(
		sf::Vector2i( get_render_target().GetWidth() / 2, get_render_target().GetHeight() / 2 ),
		get_render_target()
	);

	// Movement.
	if( m_update_velocity ) {
		m_velocity.z = (m_walk_forward ? -1.0f : 0.0f) + (m_walk_backward ? 1.0f : 0.0f);
		m_velocity.x = (m_strafe_left ? -1.0f : 0.0f) + (m_strafe_right ? 1.0f : 0.0f);

		flex::normalize( m_velocity );

		m_update_velocity = false;
	}

	m_camera.walk( (m_velocity.z * 2.0f) * delta.AsSeconds() );
	m_camera.strafe( (m_velocity.x * 2.0f) * delta.AsSeconds() );

	// Update FPS string.
	static sf::Time elapsed;
	
	elapsed += delta;

	if( elapsed >= sf::Microseconds( 1000000 / 2 ) ) {
		std::stringstream sstr;
		sstr
			<< "FPS renderer/logic: "
			<< get_render_fps()
			<< (get_shared().user_settings.is_vsync_enabled() ? " (sync)" : "")
			<< "/"
			<< get_logic_fps()
		;

		m_fps_text.SetString( sstr.str() );

		elapsed = sf::Time::Zero;
	}

	// Finalize previously prepared textures.
	m_resource_manager.finalize_prepared_textures();
}

void PlayState::render() const {
	sf::RenderWindow& target = get_render_target();

	// Clear.
	sf::Color local_sky_color = m_sky->get_local_sky_color();

	glClearColor(
		static_cast<float>( local_sky_color.r ) / 255.f,
		static_cast<float>( local_sky_color.g ) / 255.f,
		static_cast<float>( local_sky_color.b ) / 255.f,
		static_cast<float>( local_sky_color.a ) / 255.f
	);

	glEnable( GL_DEPTH_TEST );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Render sky.
	m_sky->render();

	// Render planet.
	if( m_planet_renderer ) {
		if( m_wireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}

		glCullFace( GL_BACK );

		glEnable( GL_CULL_FACE );
		m_planet_renderer->render();
		glDisable( GL_CULL_FACE );

		if( m_wireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslatef( 0, 2, -5 );
	glColor3f( 1, 0, 0 );

	//////////////// WARNING! SFML CODE MAY BEGIN HERE, SO SAVE OUR STATES //////////////////////
	glDisable( GL_DEPTH_TEST );

	target.PushGLStates();

	glEnableClientState( GL_VERTEX_ARRAY ); // SFML needs this.
	glEnableClientState( GL_TEXTURE_COORD_ARRAY ); // SFML needs this.
	glBindBuffer( GL_ARRAY_BUFFER, 0 ); // Otherwise SFML will f*ck the driver.

	// FPS.
	target.Draw( m_fps_text );

	// Render GUI.
	sfg::Renderer::Get().Display( target );

	// Restore SFML's states and render everything.
	target.PopGLStates();

	target.Display();
}

void PlayState::request_chunks( const ViewCuboid& cuboid ) {
	std::size_t num_requests = 0; // XXX
	flex::Planet::Vector runner;

	for( runner.z = cuboid.z; runner.z < cuboid.z + cuboid.width; ++runner.z ) {
		for( runner.y = cuboid.y; runner.y < cuboid.y + cuboid.width; ++runner.y ) {
			for( runner.x = cuboid.x; runner.x < cuboid.x + cuboid.width; ++runner.x ) {
				flex::msg::RequestChunk req_msg;

				req_msg.set_position( runner );
				req_msg.set_timestamp( 0 );
				get_shared().client->send_message( req_msg );

				++num_requests;
			}
		}
	}

	std::stringstream sstr;
	sstr << "Requested " << num_requests << " chunks.";
	m_console->add_message( sstr.str() );
}

void PlayState::handle_message( const flex::msg::Beam& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	m_console->add_message( sf::String( "Beamed to planet " ) + msg.get_planet_name() + sf::String( "." ) );

	// Fetch planet.
	get_shared().lock_facility->lock_world( true );
	const flex::Planet* planet = get_shared().world->find_planet( msg.get_planet_name() );

	if( planet ) {
		get_shared().lock_facility->lock_planet( *planet, true );
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		m_console->add_message( "Host beamed us to an invalid planet." );
		return;
	}

	// Stop preparation thread.
	stop_and_wait_for_chunk_preparation_thread();

	// Save current planet ID.
	m_current_planet_id = msg.get_planet_name();

	// Setup preparation thread for chunks.
	launch_chunk_preparation_thread();

	// Update view cuboid.
	flex::Planet::Vector chunk_pos;
	flex::Chunk::Vector block_pos;

	if( !planet->transform( msg.get_position(), chunk_pos, block_pos ) ) {
		m_console->add_message( "Host gave invalid beam position." );
		get_shared().lock_facility->lock_planet( *planet, false );
		return;
	}

	m_view_cuboid.x = static_cast<flex::Planet::ScalarType>( chunk_pos.x - std::min( chunk_pos.x, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.y = static_cast<flex::Planet::ScalarType>( chunk_pos.y - std::min( chunk_pos.y, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.z = static_cast<flex::Planet::ScalarType>( chunk_pos.z - std::min( chunk_pos.z, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.width = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().x - chunk_pos.x ), flex::Planet::ScalarType( 10 ) );
	m_view_cuboid.height = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().y - chunk_pos.y ), flex::Planet::ScalarType( 10 ) );
	m_view_cuboid.depth = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().z - chunk_pos.z ), flex::Planet::ScalarType( 10 ) );

	// Setup resource manager.
	m_resource_manager.set_base_path( flex::ROOT_DATA_DIRECTORY + std::string( "/packages/" ) );

	// Setup planet renderer.
	m_planet_renderer.reset( new PlanetRenderer( *planet, m_resource_manager ) );
	m_planet_renderer->set_camera( m_camera );

	get_shared().lock_facility->lock_planet( *planet, false );

	// XXX 
	std::stringstream debug_msg;
	debug_msg
		<< "New cuboid: "
		<< m_view_cuboid.x << ", " << m_view_cuboid.y << ", " << m_view_cuboid.z
		<< " "
		<< m_view_cuboid.width << " * " << m_view_cuboid.height << " * " << m_view_cuboid.depth
	;
	m_console->add_message( debug_msg.str() );

	// Request chunks.
	request_chunks( m_view_cuboid );
}

void PlayState::handle_message( const flex::msg::ChunkUnchanged& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	if( !m_current_planet_id.size() ) {
		return;
	}

	get_shared().lock_facility->lock_world( true );
	const flex::Planet* planet = get_shared().world->find_planet( m_current_planet_id );

	if( !planet ) {
#if !defined( NDEBUG )
		std::cout << "WARNING: Host notified about an unchaned chunk at an invalid planet." << std::endl;
#endif
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}

	// Add chunk to preparation list.
	{
		boost::lock_guard<boost::mutex> list_lock( m_chunk_list_mutex );
		m_chunk_list.push_back( msg.get_position() );
	}

	// Notify thread.
	m_prepare_chunks_condition.notify_one();
}

void PlayState::prepare_chunks() {
	// We need a valid context for loading textures.
	sf::Context context;

	boost::unique_lock<boost::mutex> do_lock( m_prepare_chunks_mutex );

	while( m_do_prepare_chunks ) {
		// If there's no data, wait.
		m_chunk_list_mutex.lock();

		if( m_chunk_list.size() == 0 ) {
			m_chunk_list_mutex.unlock();
			m_prepare_chunks_condition.wait( do_lock );
			continue;
		}

		// Work until all chunks have been processed.
		while( m_chunk_list.size() > 0 ) {
			// Get next chunk position.
			flex::Planet::Vector chunk_pos = m_chunk_list.front();
			m_chunk_list.pop_front();

			m_chunk_list_mutex.unlock();

			// Get planet.
			get_shared().lock_facility->lock_world( true );

			const flex::Planet* planet = get_shared().world->find_planet( m_current_planet_id );
			assert( planet );
			get_shared().lock_facility->lock_planet( *planet, true );

			get_shared().lock_facility->lock_world( false );

			// Make sure chunk exists.
			if( planet->has_chunk( chunk_pos ) ) {
				// Prepare chunk in renderer.
				m_planet_renderer->prepare_chunk( chunk_pos );
			}
			else {
#if !defined( NDEBUG )
				std::cout << "Skipping, no chunk at given position." << std::endl;
#endif
			}

			get_shared().lock_facility->lock_planet( *planet, false );
			m_chunk_list_mutex.lock();
		}

		m_chunk_list_mutex.unlock();
	}

}

void PlayState::launch_chunk_preparation_thread() {
	assert( !m_prepare_chunks_thread );
	assert( m_do_prepare_chunks == false );

	m_do_prepare_chunks = true;
	m_prepare_chunks_thread.reset( new boost::thread( std::bind( &PlayState::prepare_chunks, this ) ) );
}

void PlayState::stop_and_wait_for_chunk_preparation_thread() {
	// Do nothing if thread is not running.
	if( !m_prepare_chunks_thread ) {
		return;
	}

	{
		boost::lock_guard<boost::mutex> do_lock( m_prepare_chunks_mutex );
		m_do_prepare_chunks = false;
	}

	m_prepare_chunks_condition.notify_one();
	m_prepare_chunks_thread->join();

	m_prepare_chunks_thread.reset();
}
