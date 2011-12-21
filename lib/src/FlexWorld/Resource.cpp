#include <FlexWorld/Resource.hpp>

namespace flex {

Resource::Resource( const FlexId& id ) :
	m_id( id )
{
}

Resource::~Resource() {
}

const FlexId& Resource::get_id() const {
	return m_id;
}

}
