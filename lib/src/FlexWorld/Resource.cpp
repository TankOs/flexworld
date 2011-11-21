#include <FlexWorld/Resource.hpp>

namespace flex {

Resource::Resource( const ResourceId& id ) :
	m_id( id )
{
}

Resource::~Resource() {
}

const ResourceId& Resource::get_id() const {
	return m_id;
}

}
