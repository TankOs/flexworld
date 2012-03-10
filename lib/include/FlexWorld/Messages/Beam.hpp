#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Chunk.hpp>

#include <SFML/System/Vector3.hpp>
#include <string>

namespace flex {
namespace msg {

/** Beam network message.
 */
class Beam : public Message {
	public:
		enum { MAX_PLANET_NAME_LENGTH = 24 };

		/** Ctor.
		 */
		Beam();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get planet name.
		 * @return Planet name.
		 */
		const std::string& get_planet_name() const;

		/** Get position.
		 * @return Position.
		 */
		const sf::Vector3f& get_position() const;

		/** Get heading.
		 * @return Heading.
		 */
		float get_heading() const;

		/** Get planet size.
		 * @return Planet size.
		 */
		const Planet::Vector& get_planet_size() const;

		/** Get chunk size.
		 * @return Chunk size.
		 */
		const Chunk::Vector& get_chunk_size() const;

		/** Set planet name.
		 * @param planet_name Planet name.
		 */
		void set_planet_name( const std::string& planet_name );

		/** Set position.
		 * @param position Position.
		 */
		void set_position( const sf::Vector3f& position );

		/** Set heading.
		 * @param heading Heading.
		 */
		void set_heading( float heading );

		/** Set planet size.
		 * @param planet_size Planet size.
		 */
		void set_planet_size( const Planet::Vector& planet_size );

		/** Set chunk size.
		 * @param chunk_size Chunk size.
		 */
		void set_chunk_size( const Chunk::Vector& chunk_size );

	private:
		sf::Vector3f m_position;
		Planet::Vector m_planet_size;
		Chunk::Vector m_chunk_size;
		std::string m_planet_name;
		float m_heading;
};

}
}
