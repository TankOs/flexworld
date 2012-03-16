#pragma once

#include "State.hpp"
#include "Console.hpp"
#include "Sky.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "PlanetDrawable.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Cuboid.hpp>

#include <FWSG/Node.hpp>
#include <FWSG/Renderer.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <boost/thread.hpp>

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

		void launch_chunk_preparation_thread();
		void stop_and_wait_for_chunk_preparation_thread();
		void prepare_chunks();

		void on_console_message_add();
		void update_latest_messages();

		void handle_message( const flex::msg::Beam& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::ChunkUnchanged& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::CreateEntity& msg, flex::Client::ConnectionID conn_id );

		void request_chunks( const ViewCuboid& cuboid );

		// UI.
		sfg::Desktop m_desktop;
		Console::Ptr m_console;
		bool m_has_focus;

		sf::Text m_fps_text;

		std::vector<sf::Text> m_latest_messages;
		sf::Clock m_message_timer;

		// Resources.
		ResourceManager m_resource_manager;

		// Scene.
		sg::Renderer m_renderer;
		sg::Node::Ptr m_scene_graph;
		PlanetDrawable::Ptr m_planet_drawable;

		sf::Texture m_sun_texture;
		std::unique_ptr<Sky> m_sky;

		Camera m_camera;
		ViewCuboid m_view_cuboid;

		// Backend data.
		std::string m_current_planet_id;

		// Chunk preparation thread.
		typedef std::list<flex::Planet::Vector> ChunkPositionList;

		boost::mutex m_chunk_list_mutex; // Protects the chunk position list.
		boost::mutex m_prepare_chunks_mutex; // Protects run var of chunk preparation thread.
		boost::condition_variable m_prepare_chunks_condition; // Condition for triggering preparation thread.
		std::unique_ptr<boost::thread> m_prepare_chunks_thread; // The thread.

		ChunkPositionList m_chunk_list;
		bool m_do_prepare_chunks;
		bool m_prepare_thread_ready;

		// Controls.
		sf::Vector3f m_velocity;

		bool m_update_velocity;
		bool m_walk_forward;
		bool m_walk_backward;
		bool m_strafe_left;
		bool m_strafe_right;

		// State.
		bool m_my_entity_received;
};
