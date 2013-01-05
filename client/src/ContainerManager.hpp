#pragma once

#include <map>
#include <memory>
#include <cstdint>

class ContainerController;
class ResourceManager;

namespace fw {
class Entity;
}

namespace Rocket {
namespace Core {
class Context;
class ElementDocument;
}
}

/** Manager for container GUI elements.
 */
class ContainerManager {
	public:
		/** Ctor.
		 * @param context libRocket context (reference is held).
		 * @param resource_manager Resource manager.
		 */
		ContainerManager( Rocket::Core::Context& context, ResourceManager& resource_manager );

		/** Dtor.
		 */
		~ContainerManager();

		/** Copy ctor.
		 * @param other Other.
		 */
		ContainerManager( const ContainerManager& other ) = delete;

		/** Assignment.
		 * @param other Other.
		 */
		ContainerManager& operator=( const ContainerManager& other ) = delete;

		/** Create a container for an entity.
		 * Undefined behaviour if entity has already been added or container
		 * background image can't be loaded.
		 * @param entity Entity.
		 */
		void create_container( const fw::Entity& entity );

	private:
		struct ContainerInfo {
			std::shared_ptr<ContainerController> controller;
			Rocket::Core::ElementDocument* document;
		};

		typedef std::map<uint32_t, ContainerInfo> EntityContainerInfoMap;

		Rocket::Core::Context& m_context;
		ResourceManager& m_resource_manager;
		EntityContainerInfoMap m_infos;
};
