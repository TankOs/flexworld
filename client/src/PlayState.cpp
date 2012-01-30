#include <GL/glew.h>

#include "PlayState.hpp"
#include "MenuState.hpp"
#include "Shared.hpp"

#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Config.hpp>

PlayState::PlayState( sf::RenderWindow& target ) :
	State( target ),
	m_desktop( target ),
	m_view_cuboid( 0, 0, 0, 1, 1, 1 ),
	m_console( Console::Create() )
{
}

void PlayState::init() {
	// Init GLEW.
	//glewInit();

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

	// Setup scene.
	// Sky.
	m_sky.reset( new Sky );
	m_sky->set_camera( m_camera );

	m_sun_texture.LoadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/sky/sun.png" ) );
	m_sun_texture.SetSmooth( true );
	m_sky->set_sun_texture( m_sun_texture );

	m_sky->generate_stars( 500 );

	// Setup camera.
	m_camera.set_fov( 90.0f );
	m_camera.set_aspect_ratio(
		static_cast<float>( get_render_target().GetWidth() ) / static_cast<float>( get_render_target().GetHeight() )
	);

	// Projection matrix.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	gluPerspective(
		m_camera.get_fov(),
		m_camera.get_aspect_ratio(),
		0.1f,
		100.0f
	);
	
	// Texture matrix.
	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glLoadIdentity();

	// Notify server that we're ready.
	flex::msg::Ready ready_msg;
	get_shared().client->send_message( ready_msg );
}

void PlayState::cleanup() {
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
	}

	if( give_gui ) {
		m_desktop.HandleEvent( event );
	}
}

void PlayState::update( const sf::Time& delta ) {
	// Update sky.
	m_sky->set_time_of_day( m_sky->get_time_of_day() + (delta.AsSeconds() * (0.041f / 30.0f)) );

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

	glClear( GL_COLOR_BUFFER_BIT );

	// Render sky.
	m_sky->render();

	//////////////// WARNING! SFML CODE MAY BEGIN HERE, SO SAVE OUR STATES //////////////////////
	target.PushGLStates();

	glEnableClientState( GL_VERTEX_ARRAY ); // SFML needs this.
	glEnableClientState( GL_TEXTURE_COORD_ARRAY ); // SFML needs this.
	glBindBuffer( GL_ARRAY_BUFFER, 0 ); // Otherwise SFML will f*ck the driver.

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

	// Update view cuboid.
	flex::Planet::Vector chunk_pos;
	flex::Chunk::Vector block_pos;

	if( !planet->transform( msg.get_position(), chunk_pos, block_pos ) ) {
		m_console->add_message( "Host gave invalid beam position." );
		return;
	}

	m_view_cuboid.x = static_cast<flex::Planet::ScalarType>( chunk_pos.x - std::min( chunk_pos.x, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.y = static_cast<flex::Planet::ScalarType>( chunk_pos.y - std::min( chunk_pos.y, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.z = static_cast<flex::Planet::ScalarType>( chunk_pos.z - std::min( chunk_pos.z, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.width = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().x - chunk_pos.x ), flex::Planet::ScalarType( 10 ) );
	m_view_cuboid.height = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().y - chunk_pos.y ), flex::Planet::ScalarType( 10 ) );
	m_view_cuboid.depth = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().z - chunk_pos.z ), flex::Planet::ScalarType( 10 ) );

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

	get_shared().lock_facility->lock_planet( *planet, false );
}

void PlayState::handle_message( const flex::msg::ChunkUnchanged& msg, flex::Client::ConnectionID /*conn_id*/ ) {
}
