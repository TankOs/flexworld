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

		/** Get number of cached classes.
		 * @return Number of cached classes.
		 */
		std::size_t get_num_cached_classes() const;

		/** Check if ID is valid.
		 * An ID is valid if it points to a class and not to a hole.
		 * @param id ID.
		 * @return true if valid.
		 */
		bool is_id_valid( IdType id ) const;

		/** Get class.
		 * @param id ID (must be valid).
		 * @return Class.
		 */
		const Class& get_class( IdType id ) const;

		/** Cache class.
		 * @param cls Class.
		 * @return ID.
		 */
		IdType cache( const Class& cls );

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
