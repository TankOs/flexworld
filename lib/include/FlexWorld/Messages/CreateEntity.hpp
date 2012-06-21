#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Entity.hpp>

#include <string>

namespace flex {
namespace msg {

/** CreateEntity network message.
 */
class CreateEntity : public Message {
	public:
		/** Ctor.
		 */
		CreateEntity();

		/** Set ID.
		 * @param id ID.
		 */
		void set_id( Entity::ID id );

		/** Get ID.
		 * @return ID.
		 */
		Entity::ID get_id() const;

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const Planet::Coordinate& pos );

		/** Get position.
		 * @return Position.
		 */
		const Planet::Coordinate& get_position() const;

		/** Set heading.
		 * @param heading Heading.
		 */
		void set_heading( float heading );

		/** Get heading.
		 * @return Heading.
		 */
		float get_heading() const;

		/** Set class.
		 * @param cls Class.
		 */
		void set_class( const std::string& cls );

		/** Get class.
		 * @return Class.
		 */
		const std::string& get_class() const;

		/** Check if entity has a parent and hook set.
		 * @return true if parent set.
		 */
		bool has_parent() const;

		/** Set parent entity ID.
		 * @param id ID.
		 */
		void set_parent_id( Entity::ID id );

		/** Set parent hook.
		 * @param hook Hook.
		 */
		void set_parent_hook( const std::string& hook );

		/** Get parent entity ID.
		 * @return Parent entity ID.
		 * @see has_parent
		 */
		Entity::ID get_parent_id() const;

		/** Get parent hook.
		 * @return Parent hook.
		 * @see has_parent
		 */
		const std::string& get_parent_hook() const;

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
		std::string m_class;
		std::string m_parent_hook;
		Planet::Coordinate m_position;
		Entity::ID m_id;
		Entity::ID m_parent_id;
		float m_heading;
};

}
}
