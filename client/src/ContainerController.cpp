#include "ContainerController.hpp"

#include <Rocket/Core.h>
#include <cassert>

ContainerController::ContainerController( Rocket::Core::Element& root ) :
	m_root_element( root ),
	m_background_element( nullptr ),
	m_container_element( nullptr )
{
	m_background_element = m_root_element.GetElementById( "background" );
	m_container_element = m_root_element.GetElementById( "container" );

	assert( m_background_element );
	assert( m_container_element );
}

void ContainerController::set_background_image( const std::string& filename ) {
	m_background_element->SetAttribute( "src", filename.c_str() );
}

void ContainerController::add_entity( const fw::Entity& /*entity*/ ) {
}

void ContainerController::remove_entity( const fw::Entity& /*entity*/ ) {
}

void ContainerController::update_entity( const fw::Entity& /*entity*/ ) {
}
