#include "BufferObjectGroup.hpp"

#include <cassert>

BufferObjectGroup::BufferObjectGroup( std::size_t num, int flags ) :
	m_buffer_objects( num )
{
	assert( num > 0 );

	for( std::size_t idx = 0; idx < m_buffer_objects.size(); ++idx ) {
		m_buffer_objects[idx].reset( new sg::BufferObject( flags ) );
	}
}

sg::BufferObject::Ptr BufferObjectGroup::get_buffer_object( std::size_t index ) const {
	assert( index < m_buffer_objects.size() );

	return m_buffer_objects[index];
}

std::size_t BufferObjectGroup::get_num_buffer_objects() const {
	return m_buffer_objects.size();
}
