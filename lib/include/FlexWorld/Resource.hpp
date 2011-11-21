#pragma once

#include <FlexWorld/ResourceId.hpp>

namespace flex {

/** Resource.
 * A resource is basically a node on the filesystem identified by a ResourceId.
 * This class doesn't contain resource data, but instead only identifies it.
 */
class Resource {
	public:
		/** Ctor.
		 * @param id ID.
		 */
		Resource( const ResourceId& id );

		/** Dtor.
		 */
		virtual ~Resource();

		/** Get ID.
		 * @return ID.
		 */
		const ResourceId& get_id() const;

	private:
		ResourceId m_id;
};

}
