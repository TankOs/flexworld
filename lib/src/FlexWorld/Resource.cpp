#include <FlexWorld/Resource.hpp>

#include <cassert>

namespace flex {

Resource::Resource( const FlexID& id ) :
	m_id( id )
{
	assert( id.is_valid_resource() );
}

Resource::~Resource() {
}

const FlexID& Resource::get_id() const {
	return m_id;
}

}
