#pragma once

#include <FlexWorld/Message.hpp>
#include <FlexWorld/Entity.hpp>

#include <string>

namespace fw {
namespace msg {

/** LoginOK network message.
 */
class LoginOK : public Message {
	public:
		/** Ctor.
		 */
		LoginOK();

		/** Set entity ID.
		 * @param id ID.
		 */
		void set_entity_id( Entity::ID id );

		/** Get entity ID.
		 * @return ID.
		 */
		Entity::ID get_entity_id() const;

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

	private:
		Entity::ID m_entity_id;
};

}
}
