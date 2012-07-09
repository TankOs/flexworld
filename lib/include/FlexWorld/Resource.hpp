#pragma once

#include <FlexWorld/FlexID.hpp>

namespace fw {

/** Resource.
 * A resource is basically a node on the filesystem identified by a FlexID.
 * This class doesn't contain resource data, but instead only identifies it.
 */
class Resource {
	public:
		/** Ctor.
		 * @param id ID.
		 */
		Resource( const FlexID& id );

		/** Dtor.
		 */
		virtual ~Resource();

		/** Get ID.
		 * @return ID.
		 */
		const FlexID& get_id() const;

	private:
		FlexID m_id;
};

}
