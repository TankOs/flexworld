#pragma once

#include "State.hpp"
#include "Console.hpp"
#include "Sky.hpp"
#include "Camera.hpp"

#include <FlexWorld/Client.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Cuboid.hpp>

#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics/Texture.hpp>

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

		void handle_message( const flex::msg::Beam& msg, flex::Client::ConnectionID conn_id );
		void handle_message( const flex::msg::ChunkUnchanged& msg, flex::Client::ConnectionID conn_id );

		void request_chunks( const ViewCuboid& cuboid );

		sfg::Desktop m_desktop;
		sf::Texture m_sun_texture;

		Camera m_camera;
		ViewCuboid m_view_cuboid;

		std::string m_current_planet_id;

		std::unique_ptr<Sky> m_sky;
		Console::Ptr m_console;
};
