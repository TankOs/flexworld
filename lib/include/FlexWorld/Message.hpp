#pragma once

#include <FlexWorld/MessageMeta.hpp>

#include <vector>
#include <stdexcept>

namespace flex {

class MessageMeta;

/** Network message.
 * The message holds an internal sequential buffer with the data. It ensures
 * correct packing and unpacking.
 *
 * It uses a MessageMeta object to verify operations.
 */
class Message {
	public:
		typedef std::invalid_argument ValueTypeMismatch; ///< Thrown when packing a wrong value type.
		/** Ctor.
		 * @param meta Meta info.
		 */
		Message( const MessageMeta& meta );

		/** Get buffer size.
		 * @return Size.
		 */
		std::size_t get_size() const;

		/** Get buffer offset of field.
		 * @param field Field.
		 * @return Buffer index.
		 * @throws out_of_range if invalid field index given.
		 */
		std::size_t get_field_offset( std::size_t field ) const;

		/** Get current field (pointer).
		 * @return Current field.
		 */
		std::size_t get_current_field() const;

		/** Set next field (pointer).
		 * @param field Next field.
		 * @throws std::out_of_range if field is out of range.
		 */
		void set_next_field( std::size_t field );

		/** Pack value.
		 * After packing the next field will be automagically selected, except when
		 * the last field is being packed. The field pointer will stay at that
		 * field, then.
		 * @param value Value.
		 * @return *this
		 * @throws ValueTypeMismatch if given value type wasn't expected.
		 */
		Message& operator<<( uint8_t value );
		Message& operator<<( int8_t value ); ///< @copydoc Message::operator<<( uint8_t value )
		Message& operator<<( uint16_t value ); ///< @copydoc Message::operator<<( uint8_t value )
		Message& operator<<( int16_t value ); ///< @copydoc Message::operator<<( uint8_t value )
		Message& operator<<( uint32_t value ); ///< @copydoc Message::operator<<( uint8_t value )
		Message& operator<<( int32_t value ); ///< @copydoc Message::operator<<( uint8_t value )
		Message& operator<<( const std::string& value ); ///< @copydoc Message::operator<<( uint8_t value )

	private:
		typedef std::vector<char> Buffer;
		typedef std::vector<std::size_t> IndexVector;

		inline bool check_field( MessageMeta::FieldType type );
		inline void increase_field_pointer();

		Buffer m_buffer;
		IndexVector m_indices;
		const MessageMeta& m_meta;
		std::size_t m_current_field;
};

}
