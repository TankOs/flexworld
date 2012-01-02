#pragma once

#include <FlexWorld/MessageMeta.hpp>

#include <vector>
#include <stdexcept>

namespace flex {

/** Network message.
 * The message holds an internal sequential buffer with the data. It ensures
 * correct packing and unpacking.
 *
 * It uses a MessageMeta object to verify operations.
 */
class Message {
	public:
		/** Ctor.
		 * @param meta Meta info.
		 */
		Message( const MessageMeta& meta );

		/** Get total size of buffer.
		 * @return Size.
		 */
		MessageMeta::LengthType get_size() const;

		/** Get meta.
		 * @return Meta.
		 */
		const MessageMeta& get_meta() const;

		/** Get string length.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match the method's expected type.
		 * @param field Field index.
		 * @return String length.
		 */
		MessageMeta::StringLengthType get_string_length( std::size_t field ) const;

		/** Get string.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type. Keep in mind that this operation
		 * COPIES the string data from the buffer to the caller. Therefore do this
		 * only for lightweight fields/messages.
		 * @param field Field index.
		 * @return String.
		 */
		std::string get_string( std::size_t field ) const;

		/** Get dword.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @return Dword.
		 */
		MessageMeta::DWordType get_dword( std::size_t field ) const;

		/** Get word.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @return Word.
		 */
		MessageMeta::WordType get_word( std::size_t field ) const;

		/** Get byte.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @return Byte.
		 */
		MessageMeta::ByteType get_byte( std::size_t field ) const;

		/** Set string.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @param string String.
		 */
		void set_string( std::size_t field, const std::string& string );

		/** Set dword.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @param dword Dword.
		 */
		void set_dword( std::size_t field, MessageMeta::DWordType dword );

		/** Set word.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @param word Word.
		 */
		void set_word( std::size_t field, MessageMeta::WordType word );

		/** Set byte.
		 * Undefined behaviour if field index is invalid and/or field type doesn't
		 * match this method's expected type.
		 * @param field Field index.
		 * @param byte Byte.
		 */
		void set_byte( std::size_t field, MessageMeta::ByteType byte );

	private:
		typedef std::vector<uint8_t> Buffer;
		typedef std::vector<std::size_t> IndexVector;

		Buffer m_buffer;
		IndexVector m_indices;
		const MessageMeta& m_meta;
};

}
