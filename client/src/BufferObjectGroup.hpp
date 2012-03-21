#pragma once

#include <FWSG/BufferObject.hpp>
#include <vector>
#include <memory>

/** Group of buffer objects.
 */
class BufferObjectGroup {
	public:
		typedef std::shared_ptr<BufferObjectGroup> Ptr; ///< Shared pointer.
		typedef std::shared_ptr<const BufferObjectGroup> PtrConst; ///< Shared pointer to const.

		/** Ctor.
		 * @param num Number of buffer objects initially.
		 * @param flags Flags.
		 */
		BufferObjectGroup( std::size_t num, int flags );

		/** Get nth buffer object.
		 * Undefined behaviour if index out of range.
		 * @param index Index.
		 * @return Buffer object.
		 */
		sg::BufferObject::Ptr get_buffer_object( std::size_t index ) const;

		/** Get number of buffer objects.
		 * @return Number of buffer objects.
		 */
		std::size_t get_num_buffer_objects() const;

	private:
		typedef std::vector<sg::BufferObject::Ptr> BufferObjectArray;

		BufferObjectArray m_buffer_objects;
};
