#include <GL/glew.h>

#include "PlayState.hpp"
#include "MenuState.hpp"
#include "ColorPicker.hpp"
#include "Shared.hpp"
#include "PlanetDrawable.hpp"
#include "EntityGroupNode.hpp"
#include "DebugWindow.hpp"

#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Messages/Chat.hpp>
#include <FlexWorld/Messages/Use.hpp>
#include <FlexWorld/Math.hpp>
#include <FlexWorld/Config.hpp>

#include <SFGUI/Engines/BREW.hpp>
#include <FWSG/Transform.hpp>
#include <FWSG/WireframeState.hpp>
#include <FWSG/DepthTestState.hpp>
#include <sstream>
#include <iostream>

static const float MAX_WALK_SPEED = 2.0f; // [m/s] TODO: Replace by class' walk speed.
static const float MAX_RUN_SPEED = 4.0f; // [m/s] TODO: Replace by class' walk speed.
static const float WALK_ACCELERATION = 24.0f; // x/s². TODO: Replace by class' acceleration.
static const sf::Time PICK_UP_TIME = sf::milliseconds( 250 );

PlayState::PlayState( sf::RenderWindow& target ) :
	State( target ),
	m_text_scroller( sf::Vector2f( 10.0f, static_cast<float>( target.getSize().y ) - 10.0f ) ),
	m_gui_mode( false ),
	m_has_focus( true ),
	m_scene_graph( sg::Node::create() ),
	m_view_cuboid( 0, 0, 0, 1, 1, 1 ),
	m_do_prepare_objects( false ),
	m_cancel_prepare_objects( false ),
	m_velocity( 0, 0, 0 ),
	m_target_velocity( 0, 0, 0 ),
	m_update_velocity( false ),
	m_update_eyepoint( true ),
	m_walk_forward( false ),
	m_walk_backward( false ),
	m_strafe_left( false ),
	m_strafe_right( false ),
	m_run( false ),
	m_use( false ),
	m_fly_up( false ),
	m_fly_down( false ),
	m_last_picked_entity_id( 0 ),
	m_my_entity_received( false )
{
}

void PlayState::init() {
	// Init GLEW.
	//glewInit();

	// If vsync is enabled, due to set an FPS limit.
	if( get_shared().user_settings.is_vsync_enabled() ) {
		set_render_fps( 0 );
	}
	else {
		set_render_fps( static_cast<uint16_t>( get_shared().user_settings.get_fps_limit() ) );
	}

	// Reset handler.
	get_shared().client->set_handler( *this );

	// Setup desktop.
	dynamic_cast<sfg::eng::BREW*>( &sfg::Context::Get().GetEngine() )->ResetProperties();
	m_desktop.LoadThemeFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/game.theme" ) );

	// Setup chat window.
	m_chat_window = ChatWindow::Create();
	m_chat_window->SetId( "chat" );

	m_desktop.Add( m_chat_window );

	m_chat_window->SetRequisition( sf::Vector2f( static_cast<float>( get_render_target().getSize().x ) / 2.0f, 200 ) );
	m_chat_window->SetPosition(
		sf::Vector2f(
			0.f,
			static_cast<float>( get_render_target().getSize().y ) - m_chat_window->GetAllocation().height
		)
	);

	m_chat_window->OnMessageReady.Connect( &PlayState::on_chat_message_ready, this );
	m_chat_window->OnCloseClick.Connect( &PlayState::on_chat_close_click, this );

	// XXX Create some test channels.
	m_chat_window->CreateChannel( "Broadcast" );

	// Fix notebook bug.
	m_chat_window->SetAllocation(
		sf::FloatRect(
			m_chat_window->GetAllocation().left,
			m_chat_window->GetAllocation().top,
			m_chat_window->GetAllocation().width + 1,
			m_chat_window->GetAllocation().height
		)
	);

	m_text_scroller.add_text( "*** Press F12 to save a screenshot." );
	m_text_scroller.add_text( "*** Press F3 for wireframe mode." );
	m_text_scroller.add_text( "*** Press F1 for debug window." );
	m_text_scroller.add_text( "*** FlexWorld (c) Stefan Schindler, do not distribute." );

	m_chat_window->Show( false );

	// Setup debug window.
	m_debug_window = DebugWindow::Create();
	m_debug_window->Show( false );

	m_debug_window->OnClassIDChange.Connect( &PlayState::on_debug_class_id_change, this );
	m_debug_window->OnSpawnIDChange.Connect( &PlayState::on_debug_spawn_id_change, this );

	m_desktop.Add( m_debug_window );

	// Setup UI.
	m_fps_text.setCharacterSize( 12 );

	m_crosshair_texture.loadFromFile( flex::ROOT_DATA_DIRECTORY + std::string( "/local/gui/crosshair.png" ) );
	m_crosshair_sprite = sf::Sprite( m_crosshair_texture );
	m_crosshair_sprite.setPosition(
		static_cast<float>( get_render_target().getSize().x ) / 2.0f - m_crosshair_sprite.getLocalBounds().width / 2.0f,
		static_cast<float>( get_render_target().getSize().y ) / 2.0f - m_crosshair_sprite.getLocalBounds().height / 2.0f
	);

	// Setup scene.
	m_scene_graph->set_state( sg::DepthTestState( true ) );

	// Setup camera.
	m_camera.set_fov( get_shared().user_settings.get_fov() );
	m_camera.set_aspect_ratio(
		static_cast<float>( get_render_target().getSize().x ) / static_cast<float>( get_render_target().getSize().y )
	);
	m_camera.set_pitch_clamp( 90.f );
	m_camera.set_position( sf::Vector3f( 50, 2.7f, 60 ) );
	m_camera.set_eye_offset( sf::Vector3f( 0, 1.75f, 0 ) ); // TODO Get view height from class.

	// Projection matrix.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	gluPerspective(
		m_camera.get_fov(),
		m_camera.get_aspect_ratio(),
		0.01f,
		150.0f
	);
	
	// Texture matrix.
	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Notify server that we're ready.
	flex::msg::Ready ready_msg;
	get_shared().client->send_message( ready_msg );

	// Setup resource manager.
	m_resource_manager.set_base_path( flex::ROOT_DATA_DIRECTORY + std::string( "/packages/" ) );
	m_resource_manager.set_anisotropy_level( get_shared().user_settings.get_anisotropy_level() );
	m_resource_manager.set_texture_filter( get_shared().user_settings.get_texture_filter() );

	// Reset mouse so that the initial view direction is straight.
	reset_mouse();

	// Disable GUI mode for initial settings.
	enable_gui_mode( false );
}

void PlayState::cleanup() {
	// Terminate objects preparation thread.
	stop_and_wait_for_objects_preparation_thread();

	// Close connections.
	get_shared().client->stop();

	if( get_shared().host ) {
		get_shared().host->stop();
	}

	// Run the IO service to wait until all connections have been closed.
	get_shared().io_service->run();

	// Free all backend stuff.
	get_shared().host.reset();
	get_shared().lock_facility.reset();
	get_shared().client.reset();

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
	static bool skip_next_text_event = false;
	bool give_gui = m_gui_mode;

	if( event.type == sf::Event::Closed ) {
		leave();
	}
	else if(
		event.type == sf::Event::KeyPressed &&
		event.key.code == sf::Keyboard::Escape
	) {
		// If in GUI mode just leave it.
		if( m_gui_mode ) {
			enable_gui_mode( false );
			give_gui = false;

			m_chat_window->ClearEntry();
		}
		else {
			// Otherwise leave game (TODO: confirmation dialog).
			leave( new MenuState( get_render_target() ) );

			// Cancel preparing objects.
			{
				boost::lock_guard<boost::mutex> cancel_lock( m_cancel_prepare_objects_mutex );
				m_cancel_prepare_objects = true;
			}

			{
				boost::lock_guard<boost::mutex> do_lock( m_prepare_objects_mutex );
				m_do_prepare_objects = false;
			}

			// Cancel building VBOs.
			if( m_planet_drawable ) {
				m_planet_drawable->cancel_chunk_prepare();
			}

			give_gui = false;
		}
	}

	if( event.type == sf::Event::KeyPressed ) {
		if( event.key.code == sf::Keyboard::F3 ) { // Toggle wireframe.
			const sg::WireframeState* wireframe_state = m_scene_graph->find_state<sg::WireframeState>();
			m_scene_graph->set_state( sg::WireframeState( wireframe_state ? !wireframe_state->is_set() : true ) );
		}
		else if( event.key.code == sf::Keyboard::F1 ) { // Toggle debug window.
			m_debug_window->Show( !m_debug_window->IsGloballyVisible() );
			update_gui_mode();
		}
		else if( event.key.code == sf::Keyboard::F12 ) { // Screenshot (handled in State).
			m_chat_window->AddMessage( "*** Screenshot saved.", "Status" );
		}
	}

	// Check for LostFocus and GainedFocus
	if( event.type == sf::Event::LostFocus ) {
		m_has_focus = false;
	}
	else if( event.type == sf::Event::GainedFocus ) {
		m_has_focus = true;
	}

	// Event processing when not in GUI mode, in general movement and actions.
	if( !m_gui_mode ) {
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
					m_update_velocity = true;
					break;

				case Controls::WALK_BACKWARD:
					m_walk_backward = pressed;
					m_update_velocity = true;
					break;

				case Controls::STRAFE_LEFT:
					m_strafe_left = pressed;
					m_update_velocity = true;
					break;

				case Controls::STRAFE_RIGHT:
					m_strafe_right = pressed;
					m_update_velocity = true;
					break;

				case Controls::RUN:
					m_run = pressed;
					m_update_velocity = true;
					break;

				case Controls::JUMP:
					m_fly_up = pressed;
					m_update_velocity = true;
					break;

				case Controls::CROUCH:
					m_fly_down = pressed;
					m_update_velocity = true;
					break;


				case Controls::CHAT:
					if( pressed ) {
						m_chat_window->Show( !m_chat_window->IsGloballyVisible() );
						update_gui_mode();

						if( m_chat_window->IsGloballyVisible() ) {
							skip_next_text_event = true;
							m_chat_window->FocusEntry();

							// Move mouse cursor to window to keep distances low. ;-)
							sf::Mouse::setPosition(
									sf::Vector2i(
										static_cast<int>( m_chat_window->GetAllocation().left ) + static_cast<int>( m_chat_window->GetAllocation().width / 2 ),
										static_cast<int>( m_chat_window->GetAllocation().top ) + static_cast<int>( m_chat_window->GetAllocation().height / 2 )
										),
									get_render_target()
									);
						}
					}

					break;

				case Controls::PRIMARY_ACTION:
				case Controls::SECONDARY_ACTION:
				case Controls::USE:
					{
						bool is_action = (action != Controls::USE);
						bool primary = (action == Controls::PRIMARY_ACTION);

						if( pressed ) {
							// Calc forward vector.
							sf::Vector3f forward = flex::polar_to_vector(
								flex::deg_to_rad( m_camera.get_rotation().x + 90.0f ),
								flex::deg_to_rad( -m_camera.get_rotation().y ),
								1.0f
							);

							float distance = 7.0f; // TODO Take distance from server?
							sf::Vector3f origin = m_camera.get_position() + m_camera.get_eye_offset();

							// Get planet.
							get_shared().lock_facility->lock_world( true );

							const flex::Planet* planet = get_shared().world->find_planet( m_current_planet_id );
							assert( planet != nullptr );

							get_shared().lock_facility->lock_planet( *planet, true );

							// Build list of entities to be skipped.
							std::set<flex::Entity::ID> skip_entity_ids;
							skip_entity_ids.insert( get_shared().entity_id );

							ColorPicker::Result result = ColorPicker::pick(
								origin,
								forward,
								distance,
								sg::Transform(
									sf::Vector3f(
										-m_camera.get_position().x - m_camera.get_eye_offset().x,
										-m_camera.get_position().y - m_camera.get_eye_offset().y,
										-m_camera.get_position().z - m_camera.get_eye_offset().z
									),
									sf::Vector3f(
										m_camera.get_rotation().x,
										m_camera.get_rotation().y,
										m_camera.get_rotation().z
									)
								),
								sf::Vector2i(
									get_render_target().getSize().x / 2,
									get_render_target().getSize().y / 2
								),
								*planet,
								*get_shared().world,
								skip_entity_ids,
								m_resource_manager
							);

							get_shared().lock_facility->lock_planet( *planet, false );
							get_shared().lock_facility->lock_world( false );

							if( result.m_type == ColorPicker::Result::ENTITY ) {
								m_last_picked_entity_id = result.m_entity_id;
							}

							if( is_action ) {
								if( result.m_type == ColorPicker::Result::BLOCK ) {
									// Send block action msg.
									flex::msg::BlockAction ba_msg;

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
								flex::msg::Use use_msg;

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
	}

	// Skip text entered event if requested.
	if( event.type == sf::Event::TextEntered && skip_next_text_event ) {
		give_gui = false;
		skip_next_text_event = false;
	}

	if( give_gui ) {
		m_desktop.HandleEvent( event );
	}
}

void PlayState::update( const sf::Time& delta ) {
	// Ask IO service.
	get_shared().io_service->poll();

	// Update GUI.
	m_desktop.Update( delta.asSeconds() );

	// Process mouse/keyboard input only if window has the focus.
	if( m_has_focus ) {
		// Process mouse only if not in GUI mode.
		if( !m_gui_mode ) {
			// Get mouse movement.
			sf::Vector2i mouse_delta(
				sf::Mouse::getPosition( get_render_target() ) -
				sf::Vector2i( get_render_target().getSize().x / 2, get_render_target().getSize().y / 2 )
			);

			if( mouse_delta.x != 0 || mouse_delta.y != 0 ) {
				// Reset mouse.
				reset_mouse();

				// Rotate camera.
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

		// Movement.
		if( m_update_velocity ) {
			// If we didn't receive our own entity yet, do not move (what to move anyways?).
			if( !m_my_entity_received ) {
				m_velocity.x = 0;
				m_velocity.y = 0;
				m_velocity.z = 0;
			}
			else {
				m_target_velocity.x = (m_strafe_left ? -1.0f : 0.0f) + (m_strafe_right ? 1.0f : 0.0f);
				m_target_velocity.y = (m_fly_up ? 1.0f : 0.0f) + (m_fly_down ? -1.0f : 0.0f);
				m_target_velocity.z = (m_walk_forward ? -1.0f : 0.0f) + (m_walk_backward ? 1.0f : 0.0f);

				flex::normalize( m_target_velocity );
				m_target_velocity.x *= !m_run ? MAX_WALK_SPEED : MAX_RUN_SPEED;
				m_target_velocity.y *= !m_run ? MAX_WALK_SPEED : MAX_RUN_SPEED;
				m_target_velocity.z *= !m_run ? MAX_WALK_SPEED : MAX_RUN_SPEED;
			}

			m_update_velocity = false;
		}

		if( m_velocity.x < m_target_velocity.x ) {
			m_velocity.x = std::min( m_target_velocity.x, m_velocity.x + WALK_ACCELERATION * delta.asSeconds() );
		}
		else if( m_velocity.x > m_target_velocity.x ) {
			m_velocity.x = std::max( m_target_velocity.x, m_velocity.x - WALK_ACCELERATION * delta.asSeconds() );
		}

		if( m_velocity.y < m_target_velocity.y ) {
			m_velocity.y = std::min( m_target_velocity.y, m_velocity.y + WALK_ACCELERATION * delta.asSeconds() );
		}
		else if( m_velocity.y > m_target_velocity.y ) {
			m_velocity.y = std::max( m_target_velocity.y, m_velocity.y - WALK_ACCELERATION * delta.asSeconds() );
		}

		if( m_velocity.z < m_target_velocity.z ) {
			m_velocity.z = std::min( m_target_velocity.z, m_velocity.z + WALK_ACCELERATION * delta.asSeconds() );
		}
		else if( m_velocity.z > m_target_velocity.z ) {
			m_velocity.z = std::max( m_target_velocity.z, m_velocity.z - WALK_ACCELERATION * delta.asSeconds() );
		}

		if( m_velocity.z != 0 ) {
			m_camera.walk( m_velocity.z * delta.asSeconds() );
			m_update_eyepoint = true;
		}

		if( m_velocity.x != 0 ) {
			m_camera.strafe( m_velocity.x * delta.asSeconds() );
			m_update_eyepoint = true;
		}

		if( m_velocity.y != 0 ) {
			m_camera.fly( m_velocity.y * delta.asSeconds() );
			m_update_eyepoint = true;
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
	}

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

	// Finalize resources.
	m_resource_manager.finalize_prepared_textures();
	m_resource_manager.finalize_prepared_buffer_object_groups();

	// Update scene graph.
	if( m_scene_graph ) {
		m_scene_graph->update();
	}

	// Update text scroller.
	m_text_scroller.update();
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

	// Render scene graph.
	glColor3f( 1, 1, 1 );

	// Fake light.
	glLoadIdentity();

	GLfloat ambient[] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat position[] = {-0.5f, 1.0f, 0.7f, 0.0f};

	glRotatef( m_camera.get_rotation().x, 1.0f, 0.0f, 0.0f );
	glRotatef( m_camera.get_rotation().y, 0.0f, 1.0f, 0.0f );
	/*glTranslatef( -m_camera.get_position().x, 2.0f, -m_camera.get_position().z );*/

	glLightfv( GL_LIGHT0, GL_POSITION, position );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );

	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHTING );

	m_renderer.render();

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
	if( m_chat_window->IsGloballyVisible() == false ) {
		m_text_scroller.render( target );
	}

	// FPS.
	target.draw( m_fps_text );

	// Render GUI.
	sfg::Renderer::Get().Display( target );

	target.display();

	// Restore SFML's states and render everything.
	target.popGLStates();

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
}

void PlayState::handle_message( const flex::msg::Beam& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	// When being beamed, our own entity isn't there, so freeze movement until it has arrived.
	m_my_entity_received = false;

	// Fetch planet.
	get_shared().lock_facility->lock_world( true );
	const flex::Planet* planet = get_shared().world->find_planet( msg.get_planet_name() );

	if( planet ) {
		get_shared().lock_facility->lock_planet( *planet, true );
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}

	// Stop preparation thread.
	stop_and_wait_for_objects_preparation_thread();

	// Save current planet ID.
	m_current_planet_id = msg.get_planet_name();

	// Setup preparation thread for objects.
	launch_objects_preparation_thread();

	// Update view cuboid.
	flex::Planet::Vector chunk_pos;
	flex::Chunk::Vector block_pos;

	if( !planet->transform( msg.get_position(), chunk_pos, block_pos ) ) {
		get_shared().lock_facility->lock_planet( *planet, false );
		return;
	}

	m_view_cuboid.x = static_cast<flex::Planet::ScalarType>( chunk_pos.x - std::min( chunk_pos.x, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.y = static_cast<flex::Planet::ScalarType>( chunk_pos.y - std::min( chunk_pos.y, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.z = static_cast<flex::Planet::ScalarType>( chunk_pos.z - std::min( chunk_pos.z, flex::Planet::ScalarType( 10 ) ) );
	m_view_cuboid.width = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().x - chunk_pos.x ), flex::Planet::ScalarType( 10 ) );
	m_view_cuboid.height = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().y - chunk_pos.y ), flex::Planet::ScalarType( 10 ) );
	m_view_cuboid.depth = std::min( static_cast<flex::Planet::ScalarType>( planet->get_size().z - chunk_pos.z ), flex::Planet::ScalarType( 10 ) );

	// Detach old drawables.
	if( m_planet_drawable ) {
		m_scene_graph->detach( m_planet_drawable );
	}

	if( m_entity_group_node ) {
		m_scene_graph->detach( m_entity_group_node );
	}

	m_planet_drawable = PlanetDrawable::create( *planet, m_resource_manager, m_renderer );
	m_entity_group_node = EntityGroupNode::create( m_resource_manager, m_renderer );

	m_scene_graph->attach( m_planet_drawable );
	m_scene_graph->attach( m_entity_group_node );

	get_shared().lock_facility->lock_planet( *planet, false );

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

	// Add chunk to preparation list. Check if the same chunk has already been
	// added. If so, bring to front.
	{
		boost::lock_guard<boost::mutex> list_lock( m_object_list_mutex );

		ChunkPositionList::iterator pos_iter = std::find( m_chunk_list.begin(), m_chunk_list.end(), msg.get_position() );

		if( pos_iter != m_chunk_list.end() ) {
			m_chunk_list.erase( pos_iter );
			m_chunk_list.push_front( msg.get_position() );
		}
		else {
			m_chunk_list.push_back( msg.get_position() );
		}
	}

	// Notify thread.
	m_prepare_objects_condition.notify_one();
}

void PlayState::prepare_objects() {
	// We need a valid context for loading textures.
	sf::Context context;

	boost::unique_lock<boost::mutex> do_lock( m_prepare_objects_mutex );
	sf::Clock clock;

	while( m_do_prepare_objects ) {
		// If there's no data, wait.
		m_object_list_mutex.lock();

		if( m_chunk_list.size() == 0 && m_entity_ids.size() == 0 ) {
			m_object_list_mutex.unlock();
			m_prepare_objects_condition.wait( do_lock );
			continue;
		}

		// Work until all objects have been processed.
		while( m_chunk_list.size() > 0 || m_entity_ids.size() > 0 ) {
			// Shall we cancel?
			{
				boost::lock_guard<boost::mutex> cancel_lock( m_cancel_prepare_objects_mutex );

				if( m_cancel_prepare_objects ) {
					m_chunk_list.clear();
					m_entity_ids.clear();
					continue;
				}
			}

			// Check for unprepared chunk.
			if( m_chunk_list.size() > 0 ) {
				// Get next chunk position.
				flex::Planet::Vector chunk_pos = m_chunk_list.front();
				m_chunk_list.pop_front();

				m_object_list_mutex.unlock();

				// Get planet.
				get_shared().lock_facility->lock_world( true );

				const flex::Planet* planet = get_shared().world->find_planet( m_current_planet_id );
				assert( planet );
				get_shared().lock_facility->lock_planet( *planet, true );
				get_shared().lock_facility->lock_world( false );

				// Make sure chunk exists.
				if( planet->has_chunk( chunk_pos ) ) {
					// Prepare chunk in renderer.
					//m_planet_renderer->prepare_chunk( chunk_pos );
					m_planet_drawable->prepare_chunk( chunk_pos );
				}
				else {
#if !defined( NDEBUG )
					std::cout << "Skipping, no chunk at given position." << std::endl;
#endif
				}

				get_shared().lock_facility->lock_planet( *planet, false );
			}
			else if( m_entity_ids.size() > 0 ) {
				// Get next entity ID for preparation.
				uint32_t next_entity_id = m_entity_ids.front();
				m_entity_ids.pop_front();

				m_object_list_mutex.unlock();

				// Fetch entity.
				get_shared().lock_facility->lock_world( true );

				const flex::Entity* entity = get_shared().world->find_entity( next_entity_id );
				assert( entity );

				if( entity ) {
					// Give to entity drawable.
					assert( m_entity_group_node );

					{
						boost::lock_guard<boost::mutex> ed_lock( m_entity_group_node_mutex );
						m_entity_group_node->add_entity( *entity );
					}
				}

				get_shared().lock_facility->lock_world( false );
			}

			// Go on.
			m_object_list_mutex.lock();
		}

		m_object_list_mutex.unlock();
	}

}

void PlayState::launch_objects_preparation_thread() {
	assert( !m_prepare_objects_thread );
	assert( m_do_prepare_objects == false );

	m_do_prepare_objects = true;
	m_prepare_objects_thread.reset( new boost::thread( std::bind( &PlayState::prepare_objects, this ) ) );
}

void PlayState::stop_and_wait_for_objects_preparation_thread() {
	// Do nothing if thread is not running.
	if( !m_prepare_objects_thread ) {
		return;
	}

	{
		boost::lock_guard<boost::mutex> do_lock( m_prepare_objects_mutex );
		m_do_prepare_objects = false;
	}

	m_prepare_objects_condition.notify_one();
	m_prepare_objects_thread->join();

	m_prepare_objects_thread.reset();
}

void PlayState::handle_message( const flex::msg::CreateEntity& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

#if !defined( NDEBUG )
	std::cout
		<< "Received entity #" << msg.get_id() << " (" << msg.get_class() << ") @ "
		<< msg.get_position().x << ", " << msg.get_position().y << ", " << msg.get_position().z
		<< " (" << msg.get_heading() << "°)" << std::endl
	;
#endif

	// If own entity set position.
	if( msg.get_id() == get_shared().entity_id ) {
		m_my_entity_received = true;

		m_camera.set_position( msg.get_position() );
		m_camera.set_rotation( sf::Vector3f( 0, msg.get_heading(), 0 ) );

		m_update_eyepoint = true;
	}

	// Add to entity drawable (skip own entity).
	if( m_entity_group_node && msg.get_id() != get_shared().entity_id ) {
		{
			boost::lock_guard<boost::mutex> list_lock( m_object_list_mutex );

			m_entity_ids.push_back( msg.get_id() );
		}

		// Notify thread.
		m_prepare_objects_condition.notify_one();
	}
}

void PlayState::update_gui_mode() {
	bool new_mode = m_gui_mode;

	if( m_chat_window->IsGloballyVisible() ) {
		new_mode = true;
	}
	else if( m_debug_window->IsGloballyVisible() ) {
		new_mode = true;
	}
	else {
		new_mode = false;
	}

	if( new_mode == m_gui_mode ) {
		return;
	}

	enable_gui_mode( new_mode );
}

void PlayState::enable_gui_mode( bool enable ) {
	m_gui_mode = enable;

	get_render_target().setMouseCursorVisible( m_gui_mode );
	get_render_target().setKeyRepeatEnabled( m_gui_mode );

	if( m_gui_mode ) {
		// Stop movement.
		m_walk_forward = false;
		m_walk_backward = false;
		m_strafe_left = false;
		m_strafe_right = false;
		m_fly_up = false;
		m_fly_down = false;
		m_update_velocity = true;
	}
	else {
		// Hide windows.
		m_chat_window->Show( false );
		m_debug_window->Show( false );

		reset_mouse();
	}
}

void PlayState::reset_mouse() {
	// Do nothing if window not focused.
	if( !m_has_focus ) {
		return;
	}

	sf::Mouse::setPosition(
		sf::Vector2i( get_render_target().getSize().x / 2, get_render_target().getSize().y / 2 ),
		get_render_target()
	);
}

void PlayState::on_chat_message_ready() {
	// Send chat message.
	flex::msg::Chat msg;
	
	msg.set_message( m_chat_window->GetMessage() );
	msg.set_channel( m_chat_window->GetChannelName( m_chat_window->GetActiveChannel() ) );
	msg.set_sender( "-" );

	get_shared().client->send_message( msg );
}

void PlayState::handle_message( const flex::msg::Chat& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	sf::String message = "<";

	message += msg.get_sender();
	message += "> ";
	message += msg.get_message();

	m_chat_window->AddMessage( message, msg.get_channel() );
	m_text_scroller.add_text( "[" + msg.get_channel() + "] " + message );
}

void PlayState::on_chat_close_click() {
	m_chat_window->Show( false );
	update_gui_mode();
}

void PlayState::handle_message( const flex::msg::DestroyBlock& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

	flex::Planet::Vector chunk_pos;

	// Get planet.
	get_shared().lock_facility->lock_world( true );
	const flex::Planet* planet = get_shared().world->find_planet( m_current_planet_id );

	if( !planet ) {
#if !defined( NDEBUG )
		std::cout << "WARNING: Host destroyed a block at a planet I don't have." << std::endl;
#endif
	}
	else {
		get_shared().lock_facility->lock_planet( *planet, true );

		// Get chunk position.
		chunk_pos.x = static_cast<flex::Planet::ScalarType>( msg.get_block_position().x / planet->get_chunk_size().x );
		chunk_pos.y = static_cast<flex::Planet::ScalarType>( msg.get_block_position().y / planet->get_chunk_size().y );
		chunk_pos.z = static_cast<flex::Planet::ScalarType>( msg.get_block_position().z / planet->get_chunk_size().z );

		get_shared().lock_facility->lock_planet( *planet, false );
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}

	// Add chunk to preparation list.
	{
		boost::lock_guard<boost::mutex> list_lock( m_object_list_mutex );
		m_chunk_list.push_back( chunk_pos );
	}

	// Notify thread.
	m_prepare_objects_condition.notify_one();
}

void PlayState::handle_message( const flex::msg::SetBlock& msg, flex::Client::ConnectionID /*conn_id*/ ) {
	if( get_shared().host == nullptr ) {
		assert( 0 && "NOT IMPLEMENTED FOR MULTIPLAYER YET" );
		return;
	}

	flex::Planet::Vector chunk_pos;

	// Get planet.
	get_shared().lock_facility->lock_world( true );
	const flex::Planet* planet = get_shared().world->find_planet( m_current_planet_id );

	if( !planet ) {
#if !defined( NDEBUG )
		std::cout << "WARNING: Host destroyed a block at a planet I don't have." << std::endl;
#endif
	}
	else {
		get_shared().lock_facility->lock_planet( *planet, true );

		// Get chunk position.
		chunk_pos.x = static_cast<flex::Planet::ScalarType>( msg.get_block_position().x / planet->get_chunk_size().x );
		chunk_pos.y = static_cast<flex::Planet::ScalarType>( msg.get_block_position().y / planet->get_chunk_size().y );
		chunk_pos.z = static_cast<flex::Planet::ScalarType>( msg.get_block_position().z / planet->get_chunk_size().z );

		get_shared().lock_facility->lock_planet( *planet, false );
	}

	get_shared().lock_facility->lock_world( false );

	if( !planet ) {
		return;
	}

	// Add chunk to preparation list.
	{
		boost::lock_guard<boost::mutex> list_lock( m_object_list_mutex );
		m_chunk_list.push_back( chunk_pos );
	}

	// Notify thread.
	m_prepare_objects_condition.notify_one();
}

void PlayState::on_debug_class_id_change() {
	flex::msg::Chat chat_msg;

	chat_msg.set_message( "/set_next " + m_debug_window->GetCurrentID() );
	chat_msg.set_channel( "Status" );
	chat_msg.set_sender( "-" );

	get_shared().client->send_message( chat_msg );
}

void PlayState::on_debug_spawn_id_change() {
	flex::msg::Chat chat_msg;

	chat_msg.set_message( "/spawn " + m_debug_window->GetCurrentSpawnID() );
	chat_msg.set_channel( "Status" );
	chat_msg.set_sender( "-" );

	get_shared().client->send_message( chat_msg );
}
