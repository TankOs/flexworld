#pragma once

#include <vector>
#include <cstdint>

namespace flex {

/** Meta information for a message.
 */
class MessageMeta {
	public:
		typedef std::uint8_t ByteType; ///< Byte.
		typedef std::uint16_t WordType; ///< Word.
		typedef std::uint32_t DWordType; ///< DWord.
		typedef std::uint32_t StringLengthType; ///< String length.
		typedef std::uint32_t LengthType; ///< Message length.

		/** Field types.
		 */
		enum Field {
			BYTE = 0,
			WORD,
			DWORD,
			STRING
		};

		/** Ctor.
		 */
		MessageMeta();

		/** Get field size.
		 * @param field Field.
		 * @return Size.
		 */
		static LengthType get_field_size( Field field );

		/** Get field count.
		 * @return Field count.
		 */
		std::size_t get_num_fields() const;

		/** Get field.
		 * Undefined behaviour if index is invalid.
		 * @param index Field index.
		 * @return Field.
		 */
		Field get_field( std::size_t index ) const;

		/** Add field.
		 * @param type Type.
		 */
		void add_field( Field type );

		/** Get minimum size a message needs.
		 * This will count sizeof( Field ). STRING is handled differently, here
		 * sizeof( StringLengthType ) is used as the minimum size.
		 * @return Minimum size.
		 */
		LengthType get_minimum_size() const;

	private:
		typedef std::vector<Field> FieldVector;

		FieldVector m_fields;
		LengthType m_minimum_size;
};

}
