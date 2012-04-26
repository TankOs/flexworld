#pragma once

#include <FlexWorld/Message.hpp>

#include <string>
#include <cstdint>

namespace flex {
namespace msg {

/** Attach entity network message.
 */
class AttachEntity : public Message {
	public:
		/** Ctor.
		 */
		AttachEntity();

		void serialize( Buffer& buffer ) const;
		std::size_t deserialize( const char* buffer, std::size_t buffer_size );

		/** Get source entity ID.
		 * @return Source entity ID.
		 */
		uint32_t get_source_entity_id() const;

		/** Get target entity ID.
		 * @return Target entity ID.
		 */
		uint32_t get_target_entity_id() const;

		/** Get hook ID.
		 * @return Hook ID.
		 */
		const std::string& get_hook_id() const;

		/** Set source entity ID.
		 * @param id Source entity ID.
		 */
		void set_source_entity_id( uint32_t id );

		/** Set target entity ID.
		 * @param id Target entity ID.
		 */
		void set_target_entity_id( uint32_t id );

		/** Set hook ID.
		 * @param id Hook ID.
		 */
		void set_hook_id( const std::string& id );

	private:
		uint32_t m_source_entity_id;
		uint32_t m_target_entity_id;
		std::string m_hook_id;
};

}
}
