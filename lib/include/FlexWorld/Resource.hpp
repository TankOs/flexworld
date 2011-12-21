#pragma once

#include <FlexWorld/FlexId.hpp>

namespace flex {

/** Resource.
 * A resource is basically a node on the filesystem identified by a FlexId.
 * This class doesn't contain resource data, but instead only identifies it.
 */
class Resource {
	public:
		/** Ctor.
		 * @param id ID.
		 */
		Resource( const FlexId& id );

		/** Dtor.
		 */
		virtual ~Resource();

		/** Get ID.
		 * @return ID.
		 */
		const FlexId& get_id() const;

	private:
		FlexId m_id;
};

}
