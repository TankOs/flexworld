#pragma once

#include "State.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "TextScroller.hpp"
#include "UserInterface.hpp"
#include "SessionState.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/Cuboid.hpp>
#include <FlexWorld/Types.hpp>

#include <FWSG/Renderer.hpp>
#include <FWMS/Router.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

class PlanetDrawable;
class EntityGroupNode;
class DebugWindow;
class ObjectPreparerReader;

namespace sg {
class Node;
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
		typedef fw::Cuboid<fw::PlanetSizeType> ViewCuboid;

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

		void request_chunks( const ViewCuboid& cuboid );

		// UI.
		UserInterface m_user_interface;
		TextScroller m_text_scroller;

		bool m_has_focus;

		sf::Text m_fps_text;

		std::vector<sf::Text> m_latest_messages;
		sf::Clock m_message_timer;

		sf::Texture m_crosshair_texture;
		sf::Sprite m_crosshair_sprite;

		sf::SoundBuffer m_chat_buffer;
		sf::Sound m_chat_sound;

		// Resources.
		mutable ResourceManager m_resource_manager;

		// Scene.
		sg::Renderer m_renderer;

		std::shared_ptr<sg::Node> m_scene_graph;
		std::shared_ptr<PlanetDrawable> m_planet_drawable;
		std::shared_ptr<EntityGroupNode> m_entity_group_node;

		Camera m_camera;

		// Message system.
		ms::Router m_router;
		ObjectPreparerReader* m_object_preparer_reader;

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

		// State.
		SessionState m_session_state;
		uint32_t m_last_picked_entity_id;

		sf::Clock m_use_timer;
		sf::Clock m_forward_timer;
};
