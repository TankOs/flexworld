#pragma once

#include <vector>
#include <cstdint>

namespace flex {

/** Meta information for a message.
 */
class MessageMeta {
	public:
		/** Field types.
		 */
		enum FieldType {
			BYTE = 0,
			WORD,
			DWORD,
			STRING
		};

		/** Get size of type.
		 * @param type Type.
		 * @return Size in bytes.
		 */
		static std::size_t get_type_size( FieldType type );

		/** Ctor.
		 */
		MessageMeta();

		/** Get field count.
		 * @return Field count.
		 */
		std::size_t get_num_fields() const;

		/** Get field type.
		 * @param field Field.
		 * @return Field type.
		 * @throws out_of_range if field index is out of range.
		 */
		FieldType get_field_type( std::size_t field ) const;

		/** Add field.
		 * @param type Type.
		 */
		void add_field( FieldType type );

		/** Get minimum size a message would need.
		 * @return Minimum size.
		 */
		uint32_t get_minimum_size() const;

	private:
		typedef std::vector<FieldType> TypeVector;

		TypeVector m_types;
		uint32_t m_minimum_size;
};

}
