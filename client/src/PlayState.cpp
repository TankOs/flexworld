#define WIN32_LEAN_AND_MEAN

#include <GL/glew.h>

#include "PlayState.hpp"
#include "StateFactory.hpp"
#include "ColorPicker.hpp"
#include "Shared.hpp"
#include "PlanetDrawable.hpp"
#include "EntityGroupNode.hpp"
#include "ChatController.hpp"
#include "ContainerManager.hpp"
#include "SceneGraphReader.hpp"
#include "DebugReader.hpp"
#include "UserInterface.hpp"
#include "TextScroller.hpp"
#include "ResourceManager.hpp"
#include "SessionState.hpp"
#include "MessageHandler.hpp"
#include "SessionStateReader.hpp"
#include "HostSyncReader.hpp"
#include "CameraReader.hpp"
#include "ComponentSystemReader.hpp"
#include "MovementReader.hpp"
#include "WorldSyncReader.hpp"

#include <FlexWorld/Controllers/EntityWatchdog.hpp>
#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Messages/Chat.hpp>
#include <FlexWorld/Messages/Use.hpp>
#include <FlexWorld/Math.hpp>
#include <FlexWorld/Config.hpp>

#include <FWSG/Transform.hpp>
#include <FWSG/WireframeState.hpp>
#include <FWSG/DepthTestState.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Message.hpp>
#include <FWMS/Hash.hpp>
#include <FWCS/Controllers/ForceReset.hpp>
#include <FWCS/Controllers/Walk.hpp>
#include <FWCS/Controllers/MovementForceTransform.hpp>
#include <FWCS/Controllers/Acceleration.hpp>
#include <FWCS/Controllers/Movement.hpp>
#include <FWCS/Entity.hpp> // XXX
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <sstream>
#include <iostream>

static const sf::Time PICK_UP_TIME = sf::milliseconds( 250 );
static const sf::Time RUN_TIME = sf::milliseconds( 200 );
static const ms::HashValue KEY_PRESS_ID = ms::string_hash( "key_press" );
static const ms::HashValue KEY_RELEASE_ID = ms::string_hash( "key_release" );
static const ms::HashValue BUTTON_PRESS_ID = ms::string_hash( "button_press" );
static const ms::HashValue BUTTON_RELEASE_ID = ms::string_hash( "button_release" );
static const ms::HashValue KEY_ID = ms::string_hash( "key" );
static const ms::HashValue BUTTON_ID = ms::string_hash( "button" );

PlayState::PlayState( sf::RenderWindow& target ) :
	State( target ),
	m_resource_manager( new ResourceManager ),
	m_user_interface( new UserInterface( target, get_shared().user_settings.get_controls(), *m_resource_manager ) ),
	m_text_scroller( new TextScroller( sf::Vector2f( 10.0f, static_cast<float>( target.getSize().y ) - 10.0f ) ) ),
	m_has_focus( true ),
	m_camera(
		sf::FloatRect(
			0.0f,
			0.0f,
			static_cast<float>( target.getSize().x ),
			static_cast<float>( target.getSize().y )
		)
	),
	m_scene_graph( sg::Node::create() ),
	m_router( new ms::Router ),
	m_scene_graph_reader( nullptr ),
	m_session_state_reader( nullptr ),
	m_camera_reader( nullptr ),
	m_component_system_reader( nullptr ),
	m_movement_reader( nullptr ),
	m_world_sync_reader( nullptr ),
	m_update_eyepoint( true ),
	m_walk_forward( false ),
	m_walk_backward( false ),
	m_strafe_left( false ),
	m_strafe_right( false ),
	m_run( false ),
	m_use( false ),
	m_fly_up( false ),
	m_fly_down( false ),
	m_mouse_pointer_visible( true ),
	m_session_state( new SessionState ),
	m_last_picked_entity_id( 0 ),
	m_message_handler( new ::MessageHandler( *m_router ) )
{
}

void PlayState::init() {
	// Init session state.
	m_session_state->own_entity_id = get_shared().entity_id;

	// If vsync is enabled disable FPS limiter.
	if( get_shared().user_settings.is_vsync_enabled() ) {
		set_render_fps( 0 );
	}
	else {
		set_render_fps( static_cast<uint16_t>( get_shared().user_settings.get_fps_limit() ) );
	}

	// Reset handler.
	get_shared().client->set_handler( *this );

	// Setup text scroller.
	m_text_scroller->add_text( "*** Press F12 to save a screenshot." );
	m_text_scroller->add_text( "*** Press F3 for wireframe mode." );
	m_text_scroller->add_text( "*** FlexWorld (c) BoxBox.org. Do not distribute." );

	// Setup UI.
	m_fps_text.setCharacterSize( 12 );

	m_crosshair_texture.loadFromFile( fw::ROOT_DATA_DIRECTORY + std::string( "/local/gui/crosshair.png" ) );
	m_crosshair_sprite = sf::Sprite( m_crosshair_texture );
	m_crosshair_sprite.setPosition(
		static_cast<float>( get_render_target().getSize().x ) / 2.0f - m_crosshair_sprite.getLocalBounds().width / 2.0f,
		static_cast<float>( get_render_target().getSize().y ) / 2.0f - m_crosshair_sprite.getLocalBounds().height / 2.0f
	);

	m_user_interface->on_chat_message = boost::bind( &PlayState::on_chat_message, this, _1 );

	// Setup local sounds.
	m_chat_buffer.loadFromFile( fw::ROOT_DATA_DIRECTORY + std::string( "/local/sfx/chat.wav" ) );
	m_chat_sound.setBuffer( m_chat_buffer );

	// Setup scene.
	m_scene_graph->set_state( sg::DepthTestState( true ) );

	// Setup component system.
	m_system.create_controller<cs::ctrl::ForceReset>();
	m_system.create_controller<cs::ctrl::Walk>();
	m_system.create_controller<cs::ctrl::MovementForceTransform>();
	m_system.create_controller<cs::ctrl::Acceleration>();
	m_system.create_controller<cs::ctrl::Movement>();
	fw::ctrl::EntityWatchdog& watchdog_controller = m_system.create_controller<fw::ctrl::EntityWatchdog>();

	watchdog_controller.set_router( *m_router );

	// Setup message system.
	m_router->create_reader<DebugReader>();
	m_session_state_reader = &m_router->create_reader<SessionStateReader>();
	m_movement_reader = &m_router->create_reader<MovementReader>();
	m_component_system_reader = &m_router->create_reader<ComponentSystemReader>();
	m_world_sync_reader = &m_router->create_reader<WorldSyncReader>();
	m_scene_graph_reader = &m_router->create_reader<SceneGraphReader>();
	m_camera_reader = &m_router->create_reader<CameraReader>();

	m_session_state_reader->set_session_state( *m_session_state );
	m_session_state_reader->set_world( *get_shared().world );
	m_session_state_reader->set_lock_facility( *get_shared().lock_facility );

	m_scene_graph_reader->set_root_node( m_scene_graph );
	m_scene_graph_reader->set_world( *get_shared().world );
	m_scene_graph_reader->set_resource_manager( *m_resource_manager );
	m_scene_graph_reader->set_renderer( m_renderer );
	m_scene_graph_reader->set_lock_facility( *get_shared().lock_facility );
	m_scene_graph_reader->set_session_state( *m_session_state );

	m_camera_reader->set_camera( m_camera );
	m_camera_reader->set_world( *get_shared().world );
	m_camera_reader->set_lock_facility( *get_shared().lock_facility );

	m_component_system_reader->set_system( m_system );
	m_component_system_reader->set_world( *get_shared().world );
	m_component_system_reader->set_lock_facility( *get_shared().lock_facility );

	m_movement_reader->set_controls( get_shared().user_settings.get_controls() );

	m_world_sync_reader->set_world( *get_shared().world );
	m_world_sync_reader->set_lock_facility( *get_shared().lock_facility );

	HostSyncReader& host_sync_reader = m_router->create_reader<HostSyncReader>();
	host_sync_reader.set_client( *get_shared().client );

	// Setup camera.
	m_camera.setup_perspective_projection(
		get_shared().user_settings.get_fov(),
		static_cast<float>( get_render_target().getSize().x ) / static_cast<float>( get_render_target().getSize().y ),
		0.01f,
		350.0f
	);
	/*
	m_camera.set_pitch_clamp( 90.f );
	m_camera.set_position( sf::Vector3f( 50, 2.7f, 60 ) );
	m_camera.set_eye_offset( sf::Vector3f( 0, 1.75f, 0 ) ); // TODO Get view height from class.
	*/

	// Texture matrix.
	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Notify server that we're ready.
	fw::msg::Ready ready_msg;
	get_shared().client->send_message( ready_msg );

	// Setup resource manager.
	m_resource_manager->set_base_path( fw::ROOT_DATA_DIRECTORY + std::string( "/packages/" ) );
	m_resource_manager->set_anisotropy_level( get_shared().user_settings.get_anisotropy_level() );
	m_resource_manager->set_texture_filter( get_shared().user_settings.get_texture_filter() );

	// XXX XXX Open broadcast channel.
	m_user_interface->get_chat_controller().add_message( "Heya.", "Broadcast" );

	// Reset mouse so that the initial view direction is straight.
	reset_mouse();
	update_mouse_pointer();
}

void PlayState::cleanup() {
	// Kill router.
	m_router.reset();

	// Close connections.
	get_shared().client->stop();

	// Stop host if we're running it ourselves.
	if( get_shared().host ) {
		get_shared().host->stop();
	}

	// Run the IO service to wait until all connections have been closed.
	get_shared().io_service->run();

	// Free all backend stuff.
	get_shared().host.reset();
	get_shared().lock_facility.reset();
	get_shared().client.reset();
	get_shared().world.reset();

	// Restore old matrices.
	glMatrixMode( GL_TEXTURE );
	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	get_render_target().setMouseCursorVisible( true );

	// Restore states for SFML.
	glEnableClientState( GL_VERTEX_ARRAY ); // SFML needs this.
	glEnableClientState( GL_TEXTURE_COORD_ARRAY ); // SFML needs this.
	glEnableClientState( GL_COLOR_ARRAY ); // SFML needs this.
	glDisableClientState( GL_NORMAL_ARRAY ); // SFML needs this.
	glBindBuffer( GL_ARRAY_BUFFER, 0 ); // Otherwise SFML will f*ck the driver.
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); // Otherwise SFML will f*ck the driver.
}

void PlayState::handle_event( const sf::Event& event ) {
	// Leave game?
	if( event.type == sf::Event::Closed ) {
		leave();
	}

	// Find out what kind of event happened and hand over to message system.
	// Mouse move events are handled in update() because we are only interested
	// in the total change.
	if( event.type == sf::Event::KeyPressed ) {
		auto key_press_message = std::make_shared<ms::Message>( KEY_PRESS_ID );
		key_press_message->set_property<sf::Keyboard::Key>( KEY_ID, event.key.code );

		m_router->enqueue_message( key_press_message );
	}
	else if( event.type == sf::Event::KeyReleased ) {
		auto key_release_message = std::make_shared<ms::Message>( KEY_RELEASE_ID );
		key_release_message->set_property<sf::Keyboard::Key>( KEY_ID, event.key.code );

		m_router->enqueue_message( key_release_message );
	}
	else if( event.type == sf::Event::MouseButtonPressed ) {
		auto button_press_message = std::make_shared<ms::Message>( BUTTON_PRESS_ID );
		button_press_message->set_property<sf::Mouse::Button>( BUTTON_ID, event.mouseButton.button );

		m_router->enqueue_message( button_press_message );
	}
	else if( event.type == sf::Event::MouseButtonReleased ) {
		auto button_release_message = std::make_shared<ms::Message>( BUTTON_RELEASE_ID );
		button_release_message->set_property<sf::Mouse::Button>( BUTTON_ID, event.mouseButton.button );

		m_router->enqueue_message( button_release_message );
	}

#if 0
	// Give event to user interface.
	m_user_interface->handle_event( event );

	// Do we need to show/hide the mouse pointer?
	update_mouse_pointer();

	// If user interface is consuming events, stop processing.
	if( m_user_interface->is_consuming_events() ) {
		return;
	}

	// Leave game? TODO: Confirmation dialog.
	if(
		event.type == sf::Event::KeyPressed &&
		event.key.code == sf::Keyboard::Escape
	) {
		leave( StateFactory::create_menu_state( get_render_target() ) );
		return;
	}

	// Misc hardcoded commands.
	if( event.type == sf::Event::KeyPressed ) {
		if( event.key.code == sf::Keyboard::F3 ) { // Toggle wireframe.
			const sg::WireframeState* wireframe_state = m_scene_graph->find_state<sg::WireframeState>();
			m_scene_graph->set_state( sg::WireframeState( wireframe_state ? !wireframe_state->is_set() : true ) );
		}
		else if( event.key.code == sf::Keyboard::F12 ) { // Screenshot (handled in State).
			m_text_scroller->add_text( L"Screenshot saved." );
		}
	}

	// Check for LostFocus and GainedFocus
	if( event.type == sf::Event::LostFocus ) {
		m_has_focus = false;
		update_mouse_pointer();
	}
	else if( event.type == sf::Event::GainedFocus ) {
		m_has_focus = true;
		update_mouse_pointer();
	}

	// Check for actions.
	if( event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonPressed ) {
		Controls::Action action;
		bool pressed = false;

		if( event.type == sf::Event::MouseButtonPressed ) {
			// Mouse button action.
			pressed = true;
			action = get_shared().user_settings.get_controls().get_button_action( event.mouseButton.button );
		}
		else {
			// Key action.
			pressed = (event.type == sf::Event::KeyPressed);
			action = get_shared().user_settings.get_controls().get_key_action( event.key.code );
		}

		switch( action ) {
			case Controls::WALK_FORWARD:
				m_walk_forward = pressed;
				m_run = false;

				// If pressed twice in a short time enable running.
				if( pressed && m_forward_timer.restart() <= RUN_TIME ) {
					m_run = true;
				}

				break;

			case Controls::WALK_BACKWARD:
				m_walk_backward = pressed;
				break;

			case Controls::STRAFE_LEFT:
				m_strafe_left = pressed;
				break;

			case Controls::STRAFE_RIGHT:
				m_strafe_right = pressed;
				break;

			case Controls::JUMP:
				m_fly_up = pressed;
				break;

			case Controls::CROUCH:
				m_fly_down = pressed;
				break;

			case Controls::PRIMARY_ACTION:
			case Controls::SECONDARY_ACTION:
			case Controls::USE:
				{
					bool is_action = (action != Controls::USE);
					bool primary = (action == Controls::PRIMARY_ACTION);

					if( pressed ) {
						// Calc forward vector.
						sf::Vector3f forward = fw::polar_to_vector(
							fw::deg_to_rad( m_camera.get_transform().get_rotation().x + 90.0f ),
							fw::deg_to_rad( -m_camera.get_transform().get_rotation().y ),
							1.0f
						);

						float distance = 7.0f; // TODO Take distance from server?
						sf::Vector3f origin = m_camera.get_transform().get_translation();

						// Get planet.
						get_shared().lock_facility->lock_world( true );

						const fw::Planet* planet = get_shared().world->find_planet( m_session_state->current_planet_id );
						assert( planet != nullptr );

						get_shared().lock_facility->lock_planet( *planet, true );

						// Build list of entities to be skipped.
						std::set<fw::Entity::ID> skip_entity_ids;
						skip_entity_ids.insert( m_session_state->own_entity_id );

						ColorPicker::Result result = ColorPicker::pick(
							origin,
							forward,
							distance,
							m_camera,
							sf::Vector2i(
								get_render_target().getSize().x / 2,
								get_render_target().getSize().y / 2
							),
							*planet,
							*get_shared().world,
							skip_entity_ids,
							*m_resource_manager
						);

						get_shared().lock_facility->lock_planet( *planet, false );
						get_shared().lock_facility->lock_world( false );

						if( result.m_type == ColorPicker::Result::ENTITY ) {
							m_last_picked_entity_id = result.m_entity_id;
						}

						if( is_action ) {
							if( result.m_type == ColorPicker::Result::BLOCK ) {
								// Send block action msg.
								fw::msg::BlockAction ba_msg;

								ba_msg.set_block_position( result.m_block_position );
								ba_msg.set_facing( result.m_facing );
								ba_msg.set_primary( primary );

								get_shared().client->send_message( ba_msg );
							}
						}
						else {
							if( result.m_type == ColorPicker::Result::ENTITY ) {
								// Start use timer.
								m_use = true;
								m_use_timer.restart();
							}
							else {
								std::cout << "NOTHING TO USE THERE" << std::endl;
							}
						}
					}
					else {
						// If use key was depressed and the use key flag is still true,
						// execute use event.
						if( !is_action && m_use ) {
							fw::msg::Use use_msg;

							use_msg.set_entity_id( m_last_picked_entity_id );

							get_shared().client->send_message( use_msg );

							m_use = false;
						}
					}
				}

				break;

			default:
				break;
		}
	}
#endif
}

void PlayState::update( const sf::Time& delta ) {
	// Poll IO service.
	get_shared().io_service->poll();

	// Process mouse look only if window has focus and mouse pointer is grabbed + not visible.
	if( m_has_focus && !m_mouse_pointer_visible ) {
		sf::Vector2i mouse_delta(
			sf::Mouse::getPosition( get_render_target() ) -
			sf::Vector2i( get_render_target().getSize().x / 2, get_render_target().getSize().y / 2 )
		);

		if( mouse_delta.x != 0 || mouse_delta.y != 0 ) {
			// Reset mouse.
			reset_mouse();

			// Rotate scene graph.
			/*
			m_scene_graph->set_local_transform(
				sg::Transform(
					m_scene_graph->get_local_transform().get_translation(),
					(
						m_scene_graph->get_local_transform().get_rotation() +
						sf::Vector3f( 
							(
								(get_shared().user_settings.get_controls().is_mouse_inverted() ? -1.0f : 1.0f) *
								static_cast<float>( mouse_delta.y ) * get_shared().user_settings.get_controls().get_mouse_sensitivity() / 5.0f
							),
							static_cast<float>( mouse_delta.x ) * get_shared().user_settings.get_controls().get_mouse_sensitivity() / 5.0f,
							0.0f
						)
					),
					m_scene_graph->get_local_transform().get_scale(),
					m_scene_graph->get_local_transform().get_origin()
				)
			);
			*/
		}
	}

	/*
	if( m_has_focus && !m_mouse_pointer_visible ) {
		// Get mouse movement.
		sf::Vector2i mouse_delta(
			sf::Mouse::getPosition( get_render_target() ) -
			sf::Vector2i( get_render_target().getSize().x / 2, get_render_target().getSize().y / 2 )
		);

		if( mouse_delta.x != 0 || mouse_delta.y != 0 ) {
			// Reset mouse.
			reset_mouse();

			// Rotate scene graph.
			m_camera.turn(
				sf::Vector3f(
					(
					 (get_shared().user_settings.get_controls().is_mouse_inverted() ? -1.0f : 1.0f) *
					 static_cast<float>( mouse_delta.y ) * get_shared().user_settings.get_controls().get_mouse_sensitivity() / 5.0f
					),
					static_cast<float>( mouse_delta.x ) * get_shared().user_settings.get_controls().get_mouse_sensitivity() / 5.0f,
					0.0f
				)
			);

			m_update_eyepoint = true;
		}
	}

	// If eyepoint changed update transform of scene graph.
	if( m_update_eyepoint ) {
		m_scene_graph->set_local_transform(
			sg::Transform(
				sf::Vector3f(
					-m_camera.get_position().x - m_camera.get_eye_offset().x,
					-m_camera.get_position().y - m_camera.get_eye_offset().y,
					-m_camera.get_position().z - m_camera.get_eye_offset().z
				),
				m_camera.get_rotation(),
				sf::Vector3f( 1, 1, 1 )
			)
		);

		m_update_eyepoint = false;
	}
	*/

	// If use key is pressed and timer expired, execute pick up event.
	if( m_use && m_use_timer.getElapsedTime() >= PICK_UP_TIME ) {
		m_use = false;
		std::cout << "PICK UP" << std::endl;
	}

	// Update FPS string.
	static sf::Time elapsed;
	
	elapsed += delta;

	if( elapsed >= sf::microseconds( 1000000 / 2 ) ) {
		std::stringstream sstr;
		sstr
			<< "FPS: "
			<< get_render_fps()
			<< (get_shared().user_settings.is_vsync_enabled() ? " (sync)" : "")
		;

		m_fps_text.setString( sstr.str() );
		m_fps_text.setPosition(
			static_cast<float>( get_render_target().getSize().x ) - m_fps_text.getGlobalBounds().width - 10.0f,
			static_cast<float>( get_render_target().getSize().y ) - m_fps_text.getGlobalBounds().height - 10.0f
		);

		elapsed = sf::Time::Zero;
	}

	// Process messages.
	m_router->process_queue();

	// Process component system.
	m_system.run( delta );

	// XXX Output controlled entity position.
	auto controlled_entity = m_component_system_reader->get_controlled_entity();

	if( controlled_entity != nullptr ) {
		const auto position = controlled_entity->find_property<sf::Vector3f>( "position" );
		const auto force = controlled_entity->find_property<sf::Vector3f>( "force" );

		/*
		std::cout << "Force: "
			<< force->get_value().x << " "
			<< force->get_value().y << " "
			<< force->get_value().z << " "
			<< std::endl
		;
		std::cout << "Position: "
			<< position->get_value().x << " "
			<< position->get_value().y << " "
			<< position->get_value().z << " "
			<< std::endl
		;
		*/
	}

	// Process messages again that might have been added by the component system.
	// TODO: Needed?
	m_router->process_queue();

	// Finalize resources.
	m_resource_manager->finalize_prepared_textures();
	m_resource_manager->finalize_prepared_buffer_object_groups();

	// Update scene graph.
	if( m_scene_graph ) {
		m_scene_graph->update();
	}

	// Update text scroller.
	m_text_scroller->update();

	// Update GUI.
	m_user_interface->update( delta );
}

void PlayState::render() const {
	sf::RenderWindow& target = get_render_target();

	glClearColor(
		0x5a / 255.0f,
		0x87 / 255.0f,
		0xfc / 255.0f,
		1.0f
	);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset matrices.
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Render sky.
	//m_sky->render();

	glLoadIdentity();

	// Fake light.
	GLfloat ambient[] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat position[] = {-0.5f, 1.0f, 0.7f, 0.0f};

	// Rotate for light to be correctly placed.
	glRotatef( m_scene_graph->get_local_transform().get_rotation().x, 1.0f, 0.0f, 0.0f );
	glRotatef( m_scene_graph->get_local_transform().get_rotation().y, 0.0f, 1.0f, 0.0f );
	/*glTranslatef( -m_camera.get_position().x, 2.0f, -m_camera.get_position().z );*/

	glLightfv( GL_LIGHT0, GL_POSITION, position );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );

	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHTING );

	// Render scene graph.
	glColor3f( 1, 1, 1 );

	m_renderer.render( m_camera );

	glDisable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );

	//////////////// WARNING! SFML CODE MAY BEGIN HERE, SO SAVE OUR STATES //////////////////////
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glEnableClientState( GL_VERTEX_ARRAY ); // SFML needs this.
	glEnableClientState( GL_TEXTURE_COORD_ARRAY ); // SFML needs this.
	glEnableClientState( GL_COLOR_ARRAY ); // SFML needs this.
	glDisableClientState( GL_NORMAL_ARRAY ); // SFML needs this.
	glBindBuffer( GL_ARRAY_BUFFER, 0 ); // Otherwise SFML will f*ck the driver.
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); // Otherwise SFML will f*ck the driver.

	target.pushGLStates();

	// Latest messages.
	for( std::size_t msg_idx = 0; msg_idx < m_latest_messages.size(); ++msg_idx ) {
		target.draw( m_latest_messages[msg_idx] );
	}

	// Crosshair.
	target.draw( m_crosshair_sprite );

	// Text scroller.
	m_text_scroller->render( target );

	// User interface.
	m_user_interface->render();

	// FPS.
	target.draw( m_fps_text );

	target.display();

	// Restore SFML's states and render everything.
	target.popGLStates();
}

void PlayState::handle_message( const fw::msg::Beam& msg, fw::Client::ConnectionID conn_id ) {
	m_message_handler->handle_message( msg, conn_id );
}

void PlayState::handle_message( const fw::msg::ChunkUnchanged& msg, fw::Client::ConnectionID conn_id ) {
	m_message_handler->handle_message( msg, conn_id );
}

void PlayState::handle_message( const fw::msg::CreateEntity& msg, fw::Client::ConnectionID conn_id ) {
	m_message_handler->handle_message( msg, conn_id );
	/*
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

#if !defined( NDEBUG )
	std::cout
		<< "Received entity #" << msg.get_id() << " (" << msg.get_class() << ") @ "
		<< msg.get_position().x << ", " << msg.get_position().y << ", " << msg.get_position().z
		<< " (" << msg.get_heading() << "°)"
	;
	
	if( msg.has_parent() ) {
		std::cout << " Parent entity #" << msg.get_parent_id() << ", hook " << msg.get_parent_hook();
	}
	else {
		std::cout << " No parent entity.";
	}

	std::cout << std::endl;
#endif

	// If own entity set position.
	if( msg.get_id() == m_session_state->own_entity_id ) {
		m_session_state->own_entity_received = true;

		m_camera.set_position( msg.get_position() );
		m_camera.set_rotation( sf::Vector3f( 0, msg.get_heading(), 0 ) );

		m_update_eyepoint = true;
	}

	// Add to entity drawable. Skip if
	// - entity is attached to invisible hook
	// - entity is attached to ourself
	bool skip = false;

	if( m_entity_group_node == nullptr || msg.get_id() == m_session_state->own_entity_id ) {
		skip = true;
	}
	else if( msg.has_parent() ) {
		get_shared().lock_facility->lock_world( true );

		// Get entity and parent entity.
		const fw::Entity* ent = get_shared().world->find_entity( msg.get_id() );
		assert( ent != nullptr );

		const fw::Entity* parent_ent = ent->get_parent();
		assert( parent_ent != nullptr );

		if( parent_ent == nullptr ) {
			std::cerr
				<< "*** FATAL ERROR *** Entity #" << msg.get_id() << " shall be attached to #" << msg.get_parent_id()
				<< ", hook " << msg.get_parent_hook() << ", but parent doesn't exist in world."
				<< std::endl;
			;
			get_shared().lock_facility->lock_world( false );
			return;
		}

		// Check if entity is attached to invisible hook.
		if( *parent_ent->get_class().find_hook( msg.get_parent_hook() ) == fw::Class::INVISIBLE_HOOK ) {
			skip = true;
		}

		// Check if any parent entity is the player entity.
		while( parent_ent ) {
			if( parent_ent->get_id() == m_session_state->own_entity_id ) {
				// Check if entity is inventory. If so, use it to get contents.
				if( msg.get_parent_hook() == "inventory" ) {
					fw::msg::Use use_msg;

					use_msg.set_entity_id( msg.get_id() );
					get_shared().client->send_message( use_msg );

					// Create container for entity.
					m_user_interface->get_container_manager().create_container( *ent );
				}

				skip = true;
				break;
			}

			parent_ent = parent_ent->get_parent();
		}

		get_shared().lock_facility->lock_world( false );
	}

	if( !skip ) {
		std::shared_ptr<ms::Message> ms_message = std::make_shared<ms::Message>( ms::string_hash( "entity_invalidated" ) );
		ms_message->set_property( ms::string_hash( "id" ), msg.get_id() );

		m_router->enqueue_message( ms_message );
	}
	*/
}

void PlayState::reset_mouse() {
	// Do nothing if mouse cursor is visible.
	if( m_mouse_pointer_visible ) {
		return;
	}

	sf::Mouse::setPosition(
		sf::Vector2i( get_render_target().getSize().x / 2, get_render_target().getSize().y / 2 ),
		get_render_target()
	);
}

void PlayState::handle_message( const fw::msg::Chat& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	// Play sound.
	m_chat_sound.play();

	// Construct message with sender included.
	sf::String final_message;

	final_message += "<";
	final_message += msg.get_sender();
	final_message += "> ";
	final_message += msg.get_message();

	final_message = final_message;

	// Add to chat history.
	m_user_interface->get_chat_controller().add_message(
		final_message,
		msg.get_channel()
	);

	// Add to recent message.
	sf::String with_chan_message;

	with_chan_message += "[";
	with_chan_message += msg.get_channel();
	with_chan_message += "] ";
	with_chan_message += final_message;

	m_text_scroller->add_text( with_chan_message );
}

void PlayState::handle_message( const fw::msg::DestroyBlock& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

	fw::Planet::Vector chunk_pos;

	// Get planet.
	get_shared().lock_facility->lock_world( true );
	const fw::Planet* planet = get_shared().world->find_planet( m_session_state->current_planet_id );

	if( !planet ) {
#if !defined( NDEBUG )
		std::cout << "WARNING: Host destroyed a block at a planet I don't have." << std::endl;
#endif
	}
	else {
		get_shared().lock_facility->lock_planet( *planet, true );

		// Get chunk position.
		chunk_pos.x = static_cast<fw::Planet::ScalarType>( msg.get_block_position().x / planet->get_chunk_size().x );
		chunk_pos.y = static_cast<fw::Planet::ScalarType>( msg.get_block_position().y / planet->get_chunk_size().y );
		chunk_pos.z = static_cast<fw::Planet::ScalarType>( msg.get_block_position().z / planet->get_chunk_size().z );

		get_shared().lock_facility->lock_planet( *planet, false );
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}
}

void PlayState::handle_message( const fw::msg::SetBlock& msg, fw::Client::ConnectionID /*conn_id*/ ) {
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

	fw::Planet::Vector chunk_pos;

	// Get planet.
	get_shared().lock_facility->lock_world( true );
	const fw::Planet* planet = get_shared().world->find_planet( m_session_state->current_planet_id );

	if( !planet ) {
#if !defined( NDEBUG )
		std::cout << "WARNING: Host destroyed a block at a planet I don't have." << std::endl;
#endif
	}
	else {
		get_shared().lock_facility->lock_planet( *planet, true );

		// Get chunk position.
		chunk_pos.x = static_cast<fw::Planet::ScalarType>( msg.get_block_position().x / planet->get_chunk_size().x );
		chunk_pos.y = static_cast<fw::Planet::ScalarType>( msg.get_block_position().y / planet->get_chunk_size().y );
		chunk_pos.z = static_cast<fw::Planet::ScalarType>( msg.get_block_position().z / planet->get_chunk_size().z );

		get_shared().lock_facility->lock_planet( *planet, false );
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}
}

void PlayState::update_mouse_pointer() {
	bool old_visibility = m_mouse_pointer_visible;

	m_mouse_pointer_visible =
		m_user_interface->is_mouse_pointer_needed() ||
		!m_has_focus
	;

	if( old_visibility == m_mouse_pointer_visible ) {
		return;
	}

	get_render_target().setMouseCursorVisible( m_mouse_pointer_visible );
	get_render_target().setKeyRepeatEnabled( m_mouse_pointer_visible );

	if( !m_mouse_pointer_visible ) {
		// Reset mouse so that the eye won't rotate when leaving any GUI mode.
		reset_mouse();
	}
	else {
		// Stop all movements.
		m_walk_forward = false;
		m_walk_backward = false;
		m_strafe_left = false;
		m_strafe_right = false;
		m_fly_up = false;
		m_fly_down = false;
		m_use = false;
		m_run = false;
	}
}

void PlayState::on_chat_message( const sf::String& message ) {
	fw::msg::Chat chat_msg;

	chat_msg.set_sender( " " );
	chat_msg.set_channel( m_user_interface->get_chat_controller().get_active_channel() );
	chat_msg.set_message( message );

	get_shared().client->send_message( chat_msg );
}
