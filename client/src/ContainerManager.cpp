#include "ContainerManager.hpp"
#include "ContainerController.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/Config.hpp>

#include <Rocket/Core.h>
#include <cassert>
#include <iostream>

ContainerManager::ContainerManager( Rocket::Core::Context& context, ResourceManager& resource_manager ) :
	m_context( context ),
	m_resource_manager( resource_manager )
{
	m_context.AddReference();
}

ContainerManager::~ContainerManager() {
	EntityContainerInfoMap::iterator info_iter( m_infos.begin() );
	EntityContainerInfoMap::iterator info_iter_end( m_infos.end() );
	
	for( ; info_iter != info_iter_end; ++info_iter ) {
		info_iter->second.document->RemoveReference();
	}

	m_context.RemoveReference();
}

void ContainerManager::create_container( const fw::Entity& entity ) {
	assert( m_infos.find( entity.get_id() ) == m_infos.end() );

	// Load document.
	ContainerInfo info;

	info.document = m_context.LoadDocument(
		(fw::ROOT_DATA_DIRECTORY + std::string( "local/gui/container.rml" )).c_str()
	);
	assert( info.document );

	// Prepare controller.
	info.controller = std::shared_ptr<ContainerController>( new ContainerController( *info.document ) );

	// Store info.
	m_infos[entity.get_id()] = info;

	// Load background image if any.
	if( entity.get_class().has_container_image() ) {
		// Get full path of file.
		std::string background_image_path;

		bool result = m_resource_manager.find_path( entity.get_class().get_container_image().get_id(), background_image_path );
		assert( result );

		if( !result ) {
			std::cerr
				<< "WARNING: Container image for entity #" << entity.get_id()
				<< " (" << entity.get_class().get_id().get() << ")"
				<< " couldn't be loaded."
				<< std::endl
			;
		}
		else {
			info.controller->set_background_image( "@@" + background_image_path );
		}
	}

	info.document->Show();
}
