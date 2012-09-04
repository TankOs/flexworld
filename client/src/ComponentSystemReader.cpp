#include "ComponentSystemReader.hpp"

#include <cassert>

ComponentSystemReader::ComponentSystemReader() :
	ms::Reader(),
	m_system( nullptr )
{
}

void ComponentSystemReader::set_system( cs::System& system ) {
	m_system = &system;
}

void ComponentSystemReader::handle_message( const ms::Message& message ) {
	assert( get_router() != nullptr );
	assert( m_system != nullptr );
}
