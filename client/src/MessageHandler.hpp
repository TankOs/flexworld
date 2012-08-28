#pragma once

#include <FlexWorld/Client.hpp>

class SessionState;

/*
namespace fw {
class World;
class LockFacility;
}
*/

namespace ms {
class Router;
}

/** Message handler.
 */
class MessageHandler : public fw::Client::Handler {
	public:
		/** Ctor.
		 * @param router FWMS router.
		 */
		MessageHandler(
			/*
			SessionState& session_state,
			fw::LockFacility& lock_facility,
			fw::World& world
			*/
			ms::Router& router
		);

	private:
		void handle_message( const fw::msg::Beam& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::ChunkUnchanged& msg, fw::Client::ConnectionID conn_id );

		/*
		void handle_message( const fw::msg::CreateEntity& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::Chat& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::DestroyBlock& msg, fw::Client::ConnectionID conn_id );
		void handle_message( const fw::msg::SetBlock& msg, fw::Client::ConnectionID conn_id );

		void request_chunks( const ViewCuboid& cuboid );
		*/

		ms::Router& m_router;

		/*
		SessionState& m_session_state;
		fw::LockFacility& m_lock_facility;
		fw::World& m_world;
		*/

		/*
		ObjectPreparer& m_object_preparer;
		SceneLinker& m_scene_linker;
		fw::Client& m_client;
		*/
};
