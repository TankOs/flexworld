#pragma once

#include <vector>
#include <map>
#include <cstdint>

namespace flex {

class Class;

/** Cache for ID -> class relations.
 * Every cached class has a use (reference) counter. It's incremented with
 * every cache() call and decremented when calling forget(). As soon as 0 is
 * reached, the class is removed from the cache.
 */
class ClassCache {
	public:
		typedef uint16_t IdType; ///< Internal class ID type.

		/** Ctor.
		 */
		ClassCache();

		/** Get amount of cached classes.
		 * @return Size.
		 */
		std::size_t get_size() const;

		/** Get class.
		 * @param id ID.
		 * @return Class or nullptr.
		 */
		const Class* get_class( IdType id ) const;

		/** Cache class.
		 * @param cls Class.
		 * @return ID.
		 */
		IdType cache( const Class& cls );

		/** Get use count of a class.
		 * @param cls Class.
		 * @return Use count.
		 */
		std::size_t get_use_count( const Class& cls ) const;

		/** Forget class.
		 * @param cls Class.
		 */
		void forget( const Class& cls );

		/** Get number of holes.
		 * @return Holes.
		 */
		uint32_t get_num_holes() const;

	private:
		typedef std::vector<const Class*> ClassVector;
		typedef std::pair<IdType, uint32_t> IdUsePair;
		typedef std::map<const Class*, IdUsePair> IdUsePairMap;

		uint32_t m_num_holes;

		ClassVector m_classes;
		IdUsePairMap m_ids;
};

}
