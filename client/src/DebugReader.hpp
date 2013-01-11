#pragma once

#include <FWMS/Reader.hpp>
#include <SFML/System/Vector3.hpp>

namespace ms {
class Message;
}

/** Debug reader for watching the message bus.
 */
class DebugReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		DebugReader();

		/** Get player position.
		 * @return Player position.
		 */
		const sf::Vector3f& get_player_position() const;

	private:
		void handle_message( const ms::Message& message );

		sf::Vector3f m_player_position;
};
