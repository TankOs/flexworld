#pragma once

#include "State.hpp"
//#include "Camera.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/Types.hpp>

#include <FWU/Cuboid.hpp>
#include <FWSG/Renderer.hpp>
#include <FWSG/Camera.hpp>
#include <FWCS/System.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Clock.hpp>

class PlanetDrawable;
class EntityGroupNode;
class DebugWindow;
class DebugReader;
class SceneGraphReader;
class SessionStateReader;
class MessageHandler;
class SessionState;
class UserInterface;
class TextScroller;
class ResourceManager;
class MessageHandler;
class CameraReader;
class ComponentSystemReader;
class MovementReader;
class WorldSyncReader;

namespace sg {
class Node;
}

namespace ms {
class Router;
}

/** Play state.
 */
class PlayState : public State, fw::Client::Handler {
	public:
		/** Ctor.
		 * @param target Rendering target.
		 */
		PlayState( sf::RenderWindow& target );

	private:
		typedef util::Cuboid<fw::PlanetSizeType> ViewCuboid;

		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( const sf::Time& delta );
		void render() const;

		void reset_mouse();
		void update_mouse_pointer();

		void on_chat_message( const sf::String& message );

		void handle_message( const fw::msg::Beam& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::ChunkUnchanged& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::CreateEntity& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::Chat& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::DestroyBlock& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::SetBlock& msg, fw::Client::ConnectionID conn_id );

		// Resources.
		mutable std::unique_ptr<ResourceManager> m_resource_manager;

		// UI.
		std::unique_ptr<UserInterface> m_user_interface;
		std::unique_ptr<TextScroller> m_text_scroller;

		bool m_has_focus;

		sf::Font m_fps_font;
		sf::Text m_fps_text;

		std::vector<sf::Text> m_latest_messages;
		sf::Clock m_message_timer;

		sf::Texture m_crosshair_texture;
		sf::Sprite m_crosshair_sprite;

		sf::SoundBuffer m_chat_buffer;
		sf::Sound m_chat_sound;

		// Scene.
		sg::Renderer m_renderer;
		sg::Camera m_camera;
		std::shared_ptr<sg::Node> m_scene_graph;

		// Component system.
		cs::System m_system;

		// Message system.
		std::unique_ptr<ms::Router> m_router;
		DebugReader* m_debug_reader;
		SceneGraphReader* m_scene_graph_reader;
		SessionStateReader* m_session_state_reader;
		CameraReader* m_camera_reader;
		ComponentSystemReader* m_component_system_reader;
		MovementReader* m_movement_reader;
		WorldSyncReader* m_world_sync_reader;

		// Controls.
		bool m_update_eyepoint;

		bool m_walk_forward;
		bool m_walk_backward;
		bool m_strafe_left;
		bool m_strafe_right;
		bool m_run;
		bool m_use;
		bool m_fly_up; // XXX
		bool m_fly_down; // XXX

		bool m_mouse_pointer_visible;
		bool m_mouse_moved;

		// State.
		std::unique_ptr<SessionState> m_session_state;
		uint32_t m_last_picked_entity_id;

		sf::Clock m_use_timer;
		sf::Clock m_forward_timer;

		// Logic.
		std::unique_ptr<MessageHandler> m_message_handler;
};
