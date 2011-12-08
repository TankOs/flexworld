#pragma once

#include <vector>

namespace flex {

/** Meta information for a message.
 */
class MessageMeta {
	public:
		/** Field types.
		 */
		enum FieldType {
			INVALID = 0,
			BYTE,
			WORD,
			DWORD,
			STRING,
			_FIELD_TYPE_COUNT
		};

		/** Ctor.
		 */
		MessageMeta();

		/** Get field count.
		 * @return Field count.
		 */
		std::size_t get_field_count() const;

		/** Get field type.
		 * @param field Field.
		 * @return Type or INVALID.
		 */
		FieldType get_field_type( std::size_t field ) const;

		/** Add field.
		 * @param type Type.
		 * @return true on success.
		 */
		bool add_field( FieldType type );

	private:
		typedef std::vector<FieldType> TypeVector;

		TypeVector m_types;
};

}
