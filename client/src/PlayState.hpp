#pragma once

#include "State.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "TextScroller.hpp"
#include "UserInterface.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Cuboid.hpp>

#include <FWSG/Node.hpp>
#include <FWSG/Renderer.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <boost/thread.hpp>

class PlanetDrawable;
class EntityGroupNode;
class DebugWindow;

/** Play state.
 */
class PlayState : public State, flex::Client::Handler {
	public:
		/** Ctor.
		 * @param target Rendering target.
		 */
		PlayState( sf::RenderWindow& target );

	private:
		typedef flex::Cuboid<flex::Planet::ScalarType> ViewCuboid;

		void init();
		void cleanup();
		void handle_event( const sf::Event& event );
		void update( const sf::Time& delta );
		void render() const;

		void reset_mouse();

		void launch_objects_preparation_thread();
		void stop_and_wait_for_objects_preparation_thread();
		void prepare_objects();

		void on_debug_class_id_change();
		void on_debug_spawn_id_change();

		void handle_message( const flex::msg::Beam& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::ChunkUnchanged& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::CreateEntity& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::Chat& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::DestroyBlock& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::SetBlock& msg, flex::Client::ConnectionID conn_id );

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

		sg::Node::Ptr m_scene_graph;
		std::shared_ptr<PlanetDrawable> m_planet_drawable;

		std::shared_ptr<EntityGroupNode> m_entity_group_node;
		boost::mutex m_entity_group_node_mutex;

		Camera m_camera;
		ViewCuboid m_view_cuboid;

		// Backend data.
		std::string m_current_planet_id;

		// Object preparation thread.
		typedef std::list<flex::Planet::Vector> ChunkPositionList;
		typedef std::list<uint32_t> EntityIDList;

		boost::mutex m_object_list_mutex; // Protects the object lists.
		boost::mutex m_prepare_objects_mutex; // Protects run var of object preparation thread.
		boost::mutex m_cancel_prepare_objects_mutex; // Protects cancellation variable.
		boost::condition_variable m_prepare_objects_condition; // Condition for triggering preparation thread.
		std::unique_ptr<boost::thread> m_prepare_objects_thread; // The thread.

		ChunkPositionList m_chunk_list;
		EntityIDList m_entity_ids;

		bool m_do_prepare_objects; // Run the loop.
		bool m_cancel_prepare_objects; // Cancel the loop.

		// Controls.
		sf::Vector3f m_velocity;
		sf::Vector3f m_target_velocity;

		bool m_update_velocity;
		bool m_update_eyepoint;

		bool m_walk_forward;
		bool m_walk_backward;
		bool m_strafe_left;
		bool m_strafe_right;
		bool m_run;
		bool m_use;
		bool m_fly_up; // XXX
		bool m_fly_down; // XXX

		uint32_t m_last_picked_entity_id;

		sf::Clock m_use_timer;

		// State.
		bool m_my_entity_received;
};
